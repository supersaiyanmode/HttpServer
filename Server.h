#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <queue>
#include <string>
#include "Thread.cpp"
#include "HTTPServer.h"

class Server{
public:
    Server(int,bool=true);
    ~Server();
    
    void process(int);
    
    void run();

private:
    bool serverRunning;
    int port,serverSocket;
    HTTPServer httpServer;
};

#endif //SERVER_H 