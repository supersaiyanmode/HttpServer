#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <vector>
#include <string>
#include <map>

enum HTTPRequestMethod{METHOD_NONE, GET, POST, DELETE, PUT};

class HTTPRequest{
    HTTPRequestMethod method;
    std::string url, version, body;
    std::map<std::string, std::string> headers;
    HTTPRequest();
public:
    
    HTTPRequestMethod getMethod();
    std::string getUrl();
    const std::map<std::string, std::string> getHeaders();
    std::string getContent();
    
    void display();
    
    static HTTPRequest readFromSocket(int);
};

#endif
