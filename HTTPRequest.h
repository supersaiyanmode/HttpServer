#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <vector>
#include <string>
#include <map>

#include "CookieManager.h"

enum HTTPRequestMethod{METHOD_NONE, GET, POST, DELETE, PUT};

class HTTPMessage{
protected:
    std::string version; 
    std::string body;
    std::map<std::string, std::string> headers;
    virtual std::string getFirstLine()=0;
public:
    const std::map<std::string, std::string>& getHeaders();
    std::string getContent();
    std::string str();
};

class HTTPRequest: public HTTPMessage{
    HTTPRequestMethod method;
    std::string url;
    std::string getFirstLine();
    std::map<std::string, std::string> getParam, postParam;
    CookieSet cookieSet;
public:
    HTTPRequestMethod getMethod();
    std::string getMethodStr();
    std::string getUrl();
    
    static HTTPRequest readFromSocket(int);
};

class HTTPResponse: public HTTPMessage{
    int code;
    std::string codeStr;
    std::string statusMsg;
    std::string getFirstLine();
public:
    void setStatusCode(int);
    void setHeader(const std::string&, const std::string&);
    void setContent(const std::string&);
};

#endif
