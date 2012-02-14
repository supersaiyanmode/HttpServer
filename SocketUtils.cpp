#include <string>
#include <stdio.h>
#include "SocketUtils.h"
#ifdef WIN32
WSADATA wsaData;
int dummy = WSAStartup(MAKEWORD(2,2), &wsaData);
int read_(int sock, char* arr, int len){
    return recv(sock, arr,len,0);
}
int write_(int sock, const char* arr, int len){
    return send(sock,arr, len, 0);
}
int close_(int sock){
    return closesocket(sock);
}
WSADATA wsaData;
int dummy = WSAStartup(MAKEWORD(2,2), &wsaData);
#define socklen_t int
#else
int read_(int sock, char* arr, int len){
    return ::read(sock, arr,len);
}
int write_(int sock, const char* arr, int len){
    return ::write(sock,arr, len);
}
int close_(int sock){
    return ::close(sock);
}
#endif 

bool readLine(int socket, std::string &ret){ //separator - CRLF
    if (socket == -1)
        return false;
    ret="";
    while (1){
        char cur;
        if (read_(socket,&cur,1)<=0){
            return false;
        }
        if (cur == '\n' && ret[ret.length()-1] == '\r'){
            ret = ret.substr(0,ret.length()-1); //remove \r
            return true;
        }
        ret.append(1,cur); //not great, quick and dirty.
    }
}

bool readBytes(int socket, char *buf, int len){
    int bytesRead = 0;
    while (bytesRead < len){
        bytesRead += read_(socket, buf,len-bytesRead);
    }
}

bool writeString(int socket, const std::string& str){
    if (socket == -1)
        return false;
    return write_(socket,str.c_str(), str.length()) >= 0;
}

bool writeBytes(int socket, char *buf, int len){
    if (socket == -1)
        return false;
    return write_(socket, buf, len);
}
