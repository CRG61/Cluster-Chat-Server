#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <functional>
#include <string>
#include<json.hpp>

using namespace std;
using namespace placeholders;
using json = nlohmann::json;

class ChatServer
{
public:
    ChatServer(muduo::net::EventLoop *loop,
               const muduo::net::InetAddress &listenAddr,
               const string &nameArg,
               muduo::net::TcpServer::Option option = muduo::net::TcpServer::kNoReusePort);

    void start()
    {
        _server.start();
    }

private:
    void onConnectionCallback(const muduo::net::TcpConnectionPtr &conn);

    void onMessageCallback(const muduo::net::TcpConnectionPtr &conn,
                           muduo::net::Buffer *buffer,
                           muduo::Timestamp time);

    muduo::net::TcpServer _server;
    muduo::net::EventLoop *_ploop;

    enum _msgid
    {

    };
};

#endif
