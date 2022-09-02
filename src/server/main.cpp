#include <chatserver.h>
#include <chatservice.h>
#include <iostream>
#include <signal.h>
using namespace std;

void resetHandler(int)
{
    ChatService::getServicInstance()->serverReset();
    exit(0);
}

int main(int argc ,char **argv)
{
    if(argc <3)
    {
        cerr << "usage: IP port"<<endl;
        exit(-1);
    }

    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler);

    muduo::net::EventLoop loop; // epoll
    muduo::net::InetAddress addr(ip, port);
    // 使能端口复用
    ChatServer server(&loop,addr,"ChatServer",muduo::net::TcpServer::kReusePort);
    server.start();
    loop.loop();

    return 0;
}