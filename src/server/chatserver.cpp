/*
    网络模块
*/
#include "chatserver.h"
#include "chatservice.h"

ChatServer::ChatServer(muduo::net::EventLoop *loop,
                       const muduo::net::InetAddress &listenAddr,
                       const string &nameArg,
                       muduo::net::TcpServer::Option option)
                       // 声明时有默认参数的形参，在定义时不能再写一次默认参数
    : _server(loop, listenAddr, nameArg, option)
{
    _server.setConnectionCallback(bind(&ChatServer::onConnectionCallback, this, _1));

    _server.setMessageCallback(bind(&ChatServer::onMessageCallback, this, _1, _2, _3));

    // 设置服务器端的线程数量 1个I/O线程   7个worker线程
    // muduo会自动分配监听线程和工作线程
    _server.setThreadNum(8);
}

void ChatServer::onConnectionCallback(const muduo::net::TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        cout << "\nClient " << conn->peerAddress().toIp() <<":"<< conn->peerAddress().toPort() <<": Online " << endl;
    }
    else if (!conn->connected())
    {
        cout << "\nClient" << conn->peerAddress().toIp() <<":"<< conn->peerAddress().toPort()<<": Offline" << endl;
        // 处理客户端“异常下线”，这里的异常指的是没发送退出登录的json。
        // 如果直接kill掉telnet，这边服务器会崩
        ChatService::getServicInstance()->clientQuitException(conn);
    }
}

void ChatServer::onMessageCallback(const muduo::net::TcpConnectionPtr &conn,
                                   muduo::net::Buffer *buffer,
                                   muduo::Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf); // 反序列化

    // json get模板方法，需要转换成什么类型就用什么类型来实例化
    auto handle = ChatService::getServicInstance()->getHandler(js["msgid"].get<int>());
    handle(conn, js, time);
}
