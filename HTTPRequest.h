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
public:
    CookieSet cookies;
    const std::map<std::string, std::string>& getHeaders();
    std::string getContent();
    virtual std::string str(std::string="")=0; //pretty print purposes
};

class HTTPRequest: public HTTPMessage{
    HTTPRequestMethod method;
    std::string url;
    std::map<std::string, std::string> getParam, postParam;
public:
    HTTPRequestMethod getMethod();
    std::string getMethodStr();
    std::string getUrl();
    std::string str(std::string=""); //pretty print purposes
    const std::map<std::string, std::string> getParams();
    const std::map<std::string, std::string> postParams();
    
    
    static HTTPRequest readFromSocket(int);
};

class HTTPResponse: public HTTPMessage{
    int code;
    std::string codeStr;
    std::string statusMsg;
public:
    HTTPResponse();
    std::string str(std::string=""); //pretty print purposes
    void redirect(const std::string &);
    void setStatusCode(int);
    void setHeader(const std::string&, const std::string&);
    void setContent(const std::string&);
};

#endif
