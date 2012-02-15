#include "HTTPServer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

const std::string CRLF = "\r\n";

//###########SESSION MANAGER######################
typedef std::map<std::string, std::string> Session;

std::map<std::string,Session> sessionMap;

std::string startNewSession(){
    static int init = 0;
    static std::vector<std::string> Ids;
    if (!init++)
        std::srand((unsigned)time(0));
    bool unique = false;
    std::string id;
    while (!unique){
        std::stringstream ss;
        ss<<rand()<<rand()<<rand()<<rand();
        id = ss.str();
        unique = std::find(Ids.begin(), Ids.end(), id)==Ids.end();
    }
    Ids.push_back(id);
    
    sessionMap[id] = Session();
    
    return id;
}

//############HTTPServer##########################
HTTPServer::HTTPServer(){}

HTTPResponse HTTPServer::serve(HTTPRequest httpReq){
    std::cout<<"---------------------\n"<<httpReq.str()<<std::endl;
    
    HTTPResponse httpRes;
    
    
    if (httpReq.getUrl() == "/" && httpReq.getMethod() == GET){
        std::string content;
        std::stringstream ss;
        
        std::ifstream in("login.html");
        ss<<in.rdbuf();
        content = ss.str();
        
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/html");
        httpRes.setContent(content);
    }else if (httpReq.getUrl() == "/login" && httpReq.getMethod() == POST){
        std::cout<<"Post body: \n";
        std::cout<<httpReq.getContent()<<std::endl;
        
        std::string id = startNewSession();
        std::cout<<"Started new session: "<<id<<std::endl;
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/html");
        httpRes.setHeader("Set-Cookie","SESSIONID=\"" + id + "\"; Path=/;");
        httpRes.setContent("<meta http-equiv='Refresh' content='0; url=/files' />");
    }else if (httpReq.getUrl() == "/files" && httpReq.getMethod() == GET){
        //check the session..
        std::map<std::string,std::string> headers = httpReq.getHeaders();
        if (headers.find("cookie") == headers.end()){ //redirect
            httpRes.setHeader("Location","/");
            httpRes.setContent("");
            return httpRes;
        }
        std::string sessId = headers["cookie"];
        size_t pos = sessId.find_first_of('=');
        sessId = sessId.substr(0,pos);
        
        
        std::string content;
        std::stringstream ss;
        
        std::ifstream in("files.html");
        ss<<in.rdbuf();
        content = ss.str();
        
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/html");
        httpRes.setContent(content);
    }else if (httpReq.getUrl() == "/command" && httpReq.getMethod() == POST){
        std::string command = httpReq.getContent();
        
        httpRes.setHeader("Content-Type","application/json");
        if (command == "ls"){
            httpRes.setStatusCode(200);
            //httpRes.setContent(FileManager::);
        }
        
    }else if (httpReq.getUrl() == "/jquery" && httpReq.getMethod() == GET){
        std::string content;
        std::stringstream ss;
        
        std::ifstream in("jquery.js");
        ss<<in.rdbuf();
        content = ss.str();
        
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/javascript");
        httpRes.setContent(content);
    }else {
        httpRes.setStatusCode(404);
        httpRes.setContent("");
    }
    

    return httpRes;
}