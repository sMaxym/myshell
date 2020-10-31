#include "../include/tcp_server.h"
#include <cstring>

TcpServer::TcpServer(int port) {
    memset(&server, 0, sizeof( server ));
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    int res = bind( socket_id, (struct sockaddr *) &server, sizeof( server ) );
    if ( res == -1 ) {
        throw std::runtime_error("Failed to bind to address");
    }
}

void TcpServer::listen_socket()
{
    listen (socket_id , 1000);
}

int TcpServer::accept_socket() {
    struct sockaddr addr;
    socklen_t addrlen;
    int new_sock = accept(socket_id, &addr, &addrlen);
    sockets.push_back(new_sock);
    return new_sock;
}

std::string TcpServer::recieve(int psd) {
    char buf[1000000];
    int chars = recv(psd,buf, sizeof(buf ), 0);
    buf[chars] = '\0';
    return std::string(buf);
}

void TcpServer::write(int psd, const std::string &msg) {
    send(psd, msg.c_str(), msg.size(), 0 );
    //        std::cout << buf << std::endl;

}
