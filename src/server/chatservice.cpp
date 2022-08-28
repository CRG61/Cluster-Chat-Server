#include <chatservice.h>
#include <public.h>

ChatService::ChatService()
{
    _MsgHandlerMap.insert({LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)});
    _MsgHandlerMap.insert({REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)});
    //_MsgHandlerMap.insert({LOGOUT,})
}
 
ChatService* ChatService::getServicInstance()
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
    if(it == _MsgHandlerMap.end())
    {
        // 如果没有msgid对应的处理函数，就返回一个输出错误日志的lambda
        // 这样可以避免遇到这种情况时直接把业务模块干掉了LOG_ERROR
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp time){
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
    cout<<"do login service"<<endl;
}

// 注册业务
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPWD(pwd);

    if(_userModel.insert(user))
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