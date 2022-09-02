#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <iostream>
#include <muduo/base/Logging.h> //muduo错误日志
#include <functional>
#include <mutex>
#include <json.hpp>
#include <usermodel.h>
#include <offlinemessagemodel.h>
#include <friendmodel.h>
#include <groupmodel.h>
#include <redis.h>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;
using json = nlohmann::json;
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

// 业务类，懒汉单例
class ChatService
{
public:
    // 获取单例对象的接口
    static ChatService *getServicInstance();
    MsgHandler getHandler(int msgid);
    // json可以和string相互转换，经由序列化和反序列化
    // 登录业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 注销业务
    void logout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 客户端未注销直接退出
    void clientQuitException(const TcpConnectionPtr &conn);
    // 服务器异常终止
    void serverReset();
    // P2P聊天
    void p2pChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // 创建群
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群
    void joinGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群聊
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);

    // redis回调
    void handleRedisSubscribeMessage(int userid, string msg);

private:
    ChatService();

    // 映射Msgid和处理函数
    unordered_map<int, MsgHandler> _MsgHandlerMap;

    // 保存长链接
    unordered_map<int, TcpConnectionPtr> _TcpConnMap;

    mutex _connMutex;
    // 数据操作类对象
    UserModel _userModel;
    OfflineMessage _offlineMessageModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    // redis
    Redis _redis;
};

#endif