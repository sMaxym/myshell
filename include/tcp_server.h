#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <iostream>
#include <unistd.h>

class TcpServer {
    struct sockaddr_in server;
    int socket_id;
    std::vector<int> sockets;
public:
    TcpServer(int port);
    ~TcpServer() {close(socket_id);}
    void listen_socket();
    int accept_socket();
    std::string recieve(int psd);
    void write(int psd, const std::string& msg);
    void close_sock() { close(socket_id); }
};

#endif // TCP_SERVER_H
