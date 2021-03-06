#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <vector>
#include <string>
#include <map>

#include "CookieManager.h"

enum HTTPRequestMethod{METHOD_NONE, HTTP_GET, HTTP_POST, HTTP_HEAD, HTTP_PUT, HTTP_DELETE};

class HTTPMessage{
protected:
    std::string version; 
    std::string body;
    std::map<std::string, std::string> headers;
public:
    CookieSet cookies;
    const std::map<std::string, std::string>& getHeaders();
    std::string getContent();
    virtual std::string prettyPrint()=0; //pretty print purposes
    virtual std::string str()=0;
};

class HTTPRequest: public HTTPMessage{
    HTTPRequestMethod method;
    std::string url;
    std::map<std::string, std::string> getParam, postParam;
public:
    HTTPRequestMethod getMethod();
    std::string getMethodStr();
    std::string getUrl();
    std::string str();
    std::string prettyPrint();
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
    std::string str();
    std::string prettyPrint();
    void redirect(const std::string &);
    void setStatusCode(int);
    void setHeader(const std::string&, const std::string&);
    void setContent(const std::string&);
};

#endif
