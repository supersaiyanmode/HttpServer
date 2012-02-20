#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstring>

#include "HTTPServer.h"
#include "FileLister.h"

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
extern "C" {
    unsigned char* EMBED_DATA_files_html;
    unsigned long EMBED_DATA_files_html_LENGTH();
    unsigned long EMBED_DATA_files_html_INIT();
    unsigned long EMBED_DATA_files_html_CLOSE();
    
    unsigned char* EMBED_DATA_login_html;
    unsigned long EMBED_DATA_login_html_LENGTH();
    unsigned long EMBED_DATA_login_html_INIT();
    unsigned long EMBED_DATA_login_html_CLOSE();
}

HTTPServer::HTTPServer(){}

HTTPResponse HTTPServer::serve(HTTPRequest httpReq){
    std::cout<<httpReq.str("< ")<<std::endl;
    
    HTTPResponse httpRes;
    
    if (httpReq.getUrl() == "/" && httpReq.getMethod() == HTTP_GET){
        std::string content;
        std::stringstream ss;
        
        if (!EMBED_DATA_login_html)
            if (!EMBED_DATA_login_html_INIT())
                throw "Too bad!";
        
        content.resize(EMBED_DATA_login_html_LENGTH());
        std::memcpy(&content[0],EMBED_DATA_login_html, EMBED_DATA_login_html_LENGTH());
               
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/html");
        httpRes.setContent(content);
    }else if (httpReq.getUrl() == "/login" && httpReq.getMethod() == HTTP_POST){
        std::string id = startNewSession();
        std::cout<<"Started new session: "<<id<<std::endl;
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/html");
        httpRes.cookies["SESSIONID"]=id;
        httpRes.setContent("<meta http-equiv='Refresh' content='0; url=/files' />");
    }else if (httpReq.getUrl() == "/files" && httpReq.getMethod() == HTTP_GET){
        //check the session..
        std::string sessionId = httpReq.cookies["SESSIONID"].value();
        if (sessionMap.find(sessionId) == sessionMap.end()){ //redirect
            std::cout<<"**REDIRECT!**"<<std::endl;
            httpRes.redirect("/");
            return httpRes;
        }
        if (!EMBED_DATA_files_html)
            if (!EMBED_DATA_files_html_INIT())
                throw "Too bad!";
        
        //quick and dirty..
        std::string content;
        content.resize(EMBED_DATA_files_html_LENGTH());
        std::memcpy(&content[0],EMBED_DATA_files_html, EMBED_DATA_files_html_LENGTH());
        
        httpRes.setStatusCode(200);
        httpRes.setHeader("Content-Type","text/html");
        httpRes.setContent(content);
    }else if (httpReq.getUrl() == "/command" && httpReq.getMethod() == HTTP_POST){
        //check the session..
        std::string sessionId = httpReq.cookies["SESSIONID"].value();
        if (sessionMap.find(sessionId) == sessionMap.end()){ //error
            httpRes.setStatusCode(404); //we are sending a 404!
            httpRes.setContent("Invalid session");
            return httpRes;
        }
        Session& session = sessionMap[sessionId];
        
        std::stringstream ss(httpReq.getContent());
        std::string command;
        ss>>command;
        if (session["CURRENT_PATH"] == "")
            session["CURRENT_PATH"] = "";
        FileLister fl(".");
        fl.setPath(session["CURRENT_PATH"]);  // or preset basePath..
        httpRes.setHeader("Content-Type","text");
        if (command == "ls"){
            std::string res;
            std::vector<std::string> list(fl.get());
            for (std::vector<std::string>::iterator it=list.begin(); it!= list.end();
                 it++){
                res += *it + "\n";
            }
            httpRes.setStatusCode(200);
            httpRes.setContent(res);
        }else if (command == "cd"){
            ss>>command;
            if (fl.enterDir(command)){
                std::string res;
                std::vector<std::string> list(fl.get());
                for (std::vector<std::string>::iterator it=list.begin(); it!= list.end();
                        it++){
                    res += *it + "\n";
                }
                session["CURRENT_PATH"] = fl.getPath();
                httpRes.setStatusCode(200);
                httpRes.setContent(res);
            }else{
                httpRes.setStatusCode(404);
                httpRes.setContent("Invalid Path!");
            }
        }else if (command == "cd.."){
            if (fl.exitDir()){
                std::string res;
                std::vector<std::string> list(fl.get());
                for (std::vector<std::string>::iterator it=list.begin(); it!= list.end();
                        it++){
                    res += *it + "\n";
                }
                session["CURRENT_PATH"] = fl.getPath();
                httpRes.setStatusCode(200);
                httpRes.setContent(res);
            }else{
                httpRes.setStatusCode(404);
                httpRes.setContent("Invalid Path!");
            }
        }else {
            httpRes.setStatusCode(404);
            httpRes.setContent("Invalid command!");
        }  
    }else if (httpReq.getUrl() == "/preview"){
        //check the session..
        std::string sessionId = httpReq.cookies["SESSIONID"].value();
        if (sessionMap.find(sessionId) == sessionMap.end()){ //error
            httpRes.setStatusCode(404); //we are sending a 404!
            httpRes.setContent("Invalid session");
            return httpRes;
        }
        Session& session = sessionMap[sessionId];
        
        
    }else if (httpReq.getUrl() == "/jquery" && httpReq.getMethod() == HTTP_GET){
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
    std::cout<<httpRes.str("> ")<<std::endl;
    return httpRes;
}