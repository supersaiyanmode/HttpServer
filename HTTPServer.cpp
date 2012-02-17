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
std::ostream& operator<<(std::ostream& o, const std::map<std::string, std::string>& map);
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
    std::cout<<"---------------------\n";
    std::cout<<httpReq.str("< ")<<std::endl;
    
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
    }else if (httpReq.getUrl() == "/login"){// && httpReq.getMethod() == POST){
        std::string id = startNewSession();
        std::cout<<"Started new session: "<<id<<std::endl;
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/html");
        httpRes.cookies["SESSIONID"]=id;
        httpRes.setContent("<meta http-equiv='Refresh' content='0; url=/files' />");
    }else if (httpReq.getUrl() == "/files" && httpReq.getMethod() == GET){
        //check the session..
        std::string sessionId = httpReq.cookies["SESSIONID"].value();
        if (sessionMap.find(sessionId) == sessionMap.end()){ //redirect
            httpRes.redirect("/");
            return httpRes;
        }
        
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
            httpRes.setContent(FileManager::);
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
    }else if (httpReq.getUrl() == "/test"){
        std::string cookieStr;
        std::map<std::string, std::string> map = httpReq.getParams();
        for (std::map<std::string, std::string>::iterator it=map.begin(); 
                    it!=map.end(); it++){
            httpRes.cookies[it->first] = it->second;
        }
        cookieStr = cookieStr.substr(0,cookieStr.length()-1);
        httpRes.setHeader("Set-Cookie",cookieStr);
        httpRes.setContent("");
    }else {
        httpRes.setStatusCode(404);
        httpRes.setContent("");
    }
    std::cout<<httpRes.str("< ")<<std::endl;
    std::cout<<"---------------------\n";
    return httpRes;
}