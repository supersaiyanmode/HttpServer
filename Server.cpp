#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cctype>
#include <map>

#ifdef WIN32
    #include <windows.h>
    #include <winsock2.h>
    #define socklen_t int
#else
    #include <unistd.h>
    #include <sys/types.h> 
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
#endif 

#include "Server.h"
#include "HTTPRequest.h"
#include "SocketUtils.h"
const std::string CRLF = "\r\n";

void error(const char *msg){
    ::perror(msg);
    //exit(1);
}

Server::Server(int p,bool forceUse):port(p){
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    if (serverSocket < 0) 
        error("ERROR opening socket");
    std::memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    
    int optval = forceUse?1:0;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof optval);
    
    if (bind(serverSocket, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(serverSocket,10);
}

void Server::run(){
    serverRunning = true;
    while (serverRunning){
        sockaddr_in client;
        socklen_t clilen = sizeof(client);
        std::cout<<"Waiting for connection\n";
        int clientSocket = ::accept(serverSocket, (sockaddr*) &client, &clilen);
        if (clientSocket < 0){
            error("ERROR on accept");
        }else{
            Thread<Server,int>(*this, &Server::process,clientSocket).start();
        }
    }
}

void Server::process(int socket){
    try{
        HTTPRequest httpReq(HTTPRequest::readFromSocket(socket));
        
        std::ifstream in("test.html");
        std::stringstream content;
        content<<in.rdbuf();
        
        std::stringstream response;
        response<<"HTTP/1.0 200 OK"+CRLF;
        response<<"Content-Type: text/html"+CRLF;
        response<<"Content-Length: "<<content.str().length()<<CRLF;
        response<<CRLF;
        response<<content.str();
        writeString(socket,response.str());
    }catch(const std::string& s){
        std::cout<<s<<std::endl;
    }
}


Server::~Server(){
    //TODO: close client sockets
    //::close_(serverSocket);
    std::cout<<"Server shut down.\n";
}

