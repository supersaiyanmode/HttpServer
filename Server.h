#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <queue>
#include <string>
#include "Thread.cpp"

class Server{
public:
    Server(int,bool=true);
    ~Server();
    
    void process(int);
    
    void run();

private:
    bool serverRunning;
    int port,serverSocket;
};

#endif //SERVER_H 