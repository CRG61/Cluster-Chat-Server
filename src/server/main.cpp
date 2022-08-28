#include <chatserver.h>
#include <iostream>
using namespace std;

int main()
{
    muduo::net::EventLoop loop; // epoll
    muduo::net::InetAddress addr("127.0.0.1", 6000);
    // 使能端口复用
    ChatServer server(&loop,addr,"ChatServer",muduo::net::TcpServer::kReusePort);
    server.start();
    loop.loop();

    return 0;
}