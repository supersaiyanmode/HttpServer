#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctype.h>

#include "HTTPRequest.h"
#include "SocketUtils.h"

const std::string CRLF = "\r\n";

std::string trim(std::string str, std::string trimChars=" \t"){
    size_t endpos = str.find_last_not_of(" \t");
    if( std::string::npos != endpos )
        str = str.substr( 0, endpos+1 );
    size_t startpos = str.find_first_not_of(" \t");
    if( std::string::npos != startpos )
        str = str.substr( startpos );
    return str;
}

std::map<std::string, std::string> makeHeaders(const std::vector<std::string>& headersString){
    std::map<std::string, std::string> headers;
    for (std::vector<std::string>::const_iterator it=headersString.begin(); it!=headersString.end(); it++){
            std::string line = trim(*it), key, value;
            size_t posColon = line.find_first_of(':');
            if (std::string::npos == posColon)
                throw std::string("Bad Header: ") + *it;
            key = trim(line.substr(0,posColon));
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            value = trim(line.substr(posColon+1));
            headers[key] = value;
    }
    return headers;
}

const std::map<std::string, std::string>& HTTPMessage::getHeaders(){
    return headers;
}



std::string HTTPMessage::getContent(){
    return body;
}

std::string HTTPMessage::str(){
    std::string ret = getFirstLine() + CRLF;
    for (std::map<std::string,std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
        ret += it->first + ": " + it->second + CRLF;
    ret += CRLF;
    ret += body;
    return ret;
}


//###############HTTPRequest############################

HTTPRequest HTTPRequest::readFromSocket(int socket){
    HTTPRequest ret;
    
    std::string requestLine;
    std::map<std::string, std::string> headers;
    std::string body;
    
    //request line..
    std::string line;
    std::vector<std::string> headersVector;
    
    if (!readLine(socket,requestLine))
        throw "Can't read Request Line!";
    std::stringstream ss(requestLine);
    std::string methodStr,urlStr,versionStr;
    ss>>methodStr>>urlStr>>versionStr;
    std::transform(methodStr.begin(), methodStr.end(), methodStr.begin(), ::toupper);
    if (methodStr=="GET")
        ret.method = GET;
    else if (methodStr == "POST")
        ret.method = POST;
    else if (methodStr == "DELETE")
        ret.method = DELETE;
    else if (methodStr == "PUT")
        ret.method = PUT;
    else
        throw std::string("Bad Request") + methodStr;

    //make cookieSet
    cookieSet = 
    //separate GET params
    size_t posQmark= urlStr.find_first_of('?');
    if (posQmark == std::string::npos){
        ret.url = urlStr;
    }else{
        ret.url = urlStr.substr(0,posQmark);
        
        /*urlStr.substr(0,posQmark);
        std::string queryString = urlStr.substr(posQmark+1),param;
        size_t posAmp;
        while ((posAmp = queryString.find_first_of('&')) != std::string::npos){
            param = queryString.substr(0,posAmp);
            
            //quick and dirty
            size_t pos = param.find_first_of('=');
            getParam[param.substr(0,pos)] = param.substr(pos+1);
            
            queryString = 
        }*/
    }
    ret.version = versionStr;
    
    while(readLine(socket,line)){
        if (line == "")
            break;
        headersVector.push_back(line);
    }
    
    ret.headers = makeHeaders(headersVector);
    if (ret.method == POST){
        std::stringstream ss(ret.headers["content-length"]);
        int length;
        ss>>length;
        ret.body.resize(length);
        if (!readBytes(socket, &(ret.body[0]), length))
            throw "Unable to read fully!";
    }
    return ret;
}

std::string HTTPRequest::getFirstLine(){
    return getMethodStr() + " " + getUrl() + " " + version;
}
std::string HTTPRequest::getUrl(){
    return url;
}
HTTPRequestMethod HTTPRequest::getMethod(){
    return method;
}
std::string HTTPRequest::getMethodStr(){ //TODO: Fix this quick and dirty stuff..
    return (method==GET?"GET":method==POST?"POST":method==DELETE?"DELETE":"PUT");
}


//####################HTTPResponse###################
std::string HTTPResponse::getFirstLine(){
    return version + " " + codeStr + " " + statusMsg;
}

void HTTPResponse::setStatusCode(int c){
    //temporarily set version = HTTP/1.0
    version = "HTTP/1.0";
    statusMsg = c==200?"OK":c==404?"Not Found":"None";
    code = c;
    std::stringstream ss;
    ss<<c;
    ss>>codeStr;
}

void HTTPResponse::setHeader(const std::string& k, const std::string& v){
    headers[k] = v;
}

void HTTPResponse::setContent(const std::string& s){
    body = s;
    std::stringstream ss;
    ss<<s.length();
    
    //set the header..
    headers["Content-Length"] = ss.str();
}