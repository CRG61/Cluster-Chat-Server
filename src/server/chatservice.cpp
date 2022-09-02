#include <chatservice.h>
#include <public.h>

ChatService::ChatService()
{
    _MsgHandlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
    _MsgHandlerMap.insert({REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)});
    _MsgHandlerMap.insert({P2P_CHAT, bind(&ChatService::p2pChat, this, _1, _2, _3)});
    _MsgHandlerMap.insert({ADD_FRIEND, bind(&ChatService::addFriend, this, _1, _2, _3)});
    _MsgHandlerMap.insert({CREATE_GROUP, bind(&ChatService::createGroup, this, _1, _2, _3)});
    _MsgHandlerMap.insert({ADD_GROUP, bind(&ChatService::joinGroup, this, _1, _2, _3)});
    _MsgHandlerMap.insert({GROUP_CHAT, bind(&ChatService::groupChat, this, _1, _2, _3)});
    _MsgHandlerMap.insert({LOG_OUT_MSG, bind(&ChatService::logout, this, _1, _2, _3)});

    // redis服务器连接
    if (_redis.connect())
    {
        _redis.init_notify_handler(bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
    else
    {
        cerr << "redis connection failed" << endl;
        exit(-1);
    }
}

ChatService *ChatService::getServicInstance()
{
    // 各线程之间 堆空间共用
    // 写的是栈空间的自动变量，研究一下这个static的作用域
    static ChatService service;
    return &service;
}

MsgHandler ChatService::getHandler(int msgid)
{
    // 不直接用[]，避免添加不存在的msgid
    auto it = _MsgHandlerMap.find(msgid);
    if (it == _MsgHandlerMap.end())
    {
        // 如果没有msgid对应的处理函数，就返回一个输出错误日志的lambda
        // 这样可以避免遇到这种情况时直接把业务模块干掉了LOG_ERROR
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time)
        {
            LOG_ERROR << "msgid" << msgid << "no such service type";
        };
    }
    else
    {
        return _MsgHandlerMap.at(msgid);
    }
}

// 登录业务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string pwd = js["password"];
    int id = js["id"]; //.get<int>()

    User user = _userModel.query(id);
    if (user.getId() == id && user.getPWD() == pwd)
    {
        if (user.getState() == "online")
        {
            //用户在线，不能重复登录（可以继续实现为可以选择是否把前面的挤下线）
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "账号已登录，不能重复登录";
            conn->send(response.dump());
        }
        else
        {
            user.setState("online");
            if (_userModel.updateState(user))
            {
                _redis.subscribe(id);
                {
                    lock_guard<mutex> lock(_connMutex);
                    _TcpConnMap.insert({id, conn});
                }
                json response;
                response["msgid"] = LOGIN_MSG_ACK;
                response["errno"] = 0;
                response["name"] = user.getName();
                response["id"] = user.getId();

                // 查询是否有离线消息
                vector<string> vOff = _offlineMessageModel.query(id);
                if (!vOff.empty())
                {
                    // 讲道理这里直接写message就行
                    response["offlinemsg"] = vOff;
                    _offlineMessageModel.remove(id);
                }

                // 查询好友信息
                vector<User> vFri = _friendModel.query(id);
                if (!vFri.empty())
                {
                    vector<string> vsFri;
                    for (User user : vFri) //这里也可以写成引用
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        vsFri.push_back(js.dump());
                    }
                    response["friendsinfo"] = vsFri;
                }

                // 查询群组信息
                vector<Group> vGroup = _groupModel.queryGroups(id);
                if (!vGroup.empty())
                {
                    vector<string> vsGrp;
                    for (Group grp : vGroup)
                    {
                        json js;
                        js["id"] = grp.getGID();
                        js["groupdesc"] = grp.getGDesc();
                        js["groupname"] = grp.getGName();
                        vector<string> vguser;
                        vector<GroupUser> vuser = grp.getGUsers();
                        for (GroupUser guser : vuser)
                        {
                            json js;
                            js["id"] = guser.getId();
                            js["name"] = guser.getName();
                            js["state"] = guser.getState();
                            js["role"] = guser.getRole();
                            vguser.push_back(js.dump());
                        }
                        js["users"] = vguser;
                        vsGrp.push_back(js.dump());
                    }
                    response["groups"] = vsGrp;
                }
                conn->send(response.dump());
            }
            else
            {
                json response;
                response["msgid"] = LOGIN_MSG_ACK;
                response["errno"] = 4;
                response["errmsg"] = "mysql更新失败";
                conn->send(response.dump());
            }
        }
    }
    else
    {
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 3;
        response["errmsg"] = "用户名或密码错误";
        conn->send(response.dump());
    }
}

// 注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPWD(pwd);

    if (_userModel.insert(user))
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0; //放在这个位置是因为：如果errno不为0，就不需要继续读其他字段了
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::clientQuitException(const TcpConnectionPtr &conn)
{
    for (auto it = _TcpConnMap.begin(); it != _TcpConnMap.end(); ++it)
    { //讲道理这里也可以直接写find啊？？
        if (it->second == conn)
        {
            User user;
            user.setId(it->first);
            user.setState("offline");
            _userModel.updateState(user);
            {
                lock_guard<mutex> lock(_connMutex);
                _TcpConnMap.erase(it);
            }
            _redis.unsubscribe(it->first);
            break;
        }
    }
    conn->shutdown();
}

void ChatService::p2pChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();
    {
        // 这里的逻辑是有问题的，按理说对方要下线是不能阻止的
        // 还有一个问题，要阻止给自己发消息
        // 还有凭什么可以一个TCP登两个号
        lock_guard<mutex> lock(_connMutex);
        auto it = _TcpConnMap.find(toid);
        if (it != _TcpConnMap.end())
        {
            // 对方在线
            {
                it->second->send(js.dump());
                return;
            }
        }
    }

    // 对方不在线，暂存消息
    {
        User user = _userModel.query(toid);
        if (user.getState() == "online")
        {
            _redis.publish(toid, js.dump());
        }
        else
        {
            _offlineMessageModel.insert(toid, js.dump());
        }
    }
}

void ChatService::serverReset()
{
    _userModel.reset();
}

// 添加好友
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"]; // get<int>(); 似乎是不需要的
    int friendid = js["friendid"];
    _friendModel.insert(userid, friendid);
}

// 创建群
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"];
    Group group;
    group.setGName(js["groupname"]);
    group.setGDesc(js["groupdesc"]);
    json response;
    if (_groupModel.createGroup(group, userid))
    {
        response["groupID"] = group.getGID();
        response["msgid"] = GROUP_ACK_MSG;
        response["errno"] = 0;
    }
    else
    {
        response["msgid"] = GROUP_ACK_MSG;
        response["errno"] = 1;
        response["errmsg"] = "Group creation failed.";
    }
    conn->send(response.dump());
}

// 加入群
void ChatService::joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"];
    int groupid = js["groupid"];
    _groupModel.addGroup(userid, groupid, "member");
    json response;
    response["msgid"] = GROUP_ACK_MSG;
    response["errno"] = 0;
    conn->send(response.dump());
}

// 群聊
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"];
    int groupid = js["groupid"];
    vector<int> vuserid = _groupModel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for (int id : vuserid)
    {
        auto it = _TcpConnMap.find(id);
        if (it != _TcpConnMap.end())
        {
            // 用户当前在线，转发消息
            it->second->send(js.dump());
        }
        else
        {
            // 首先查数据库state状态
            User user;
            user = _userModel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                _offlineMessageModel.insert(id, js.dump());
            }
        }
    }
}

// 注销
void ChatService::logout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 无回执，id,msgid
    int id = js["id"];
    /*
    // 这么写不好，需要额外连一次数据库
    User user = _userModel.query(id);
    */
    User user(id);
    _userModel.updateState(user);

    _redis.unsubscribe(id);
}

void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _TcpConnMap.find(userid);
    if (it != _TcpConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMessageModel.insert(userid, msg);
}