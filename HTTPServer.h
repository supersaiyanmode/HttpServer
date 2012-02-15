#ifndef HTTPSERVER_H
#define HTTPSERVER_H
#include <map>
#include "HTTPRequest.h"

class HTTPServer{
public:
    HTTPServer();
    HTTPResponse serve(HTTPRequest);
};

#endif