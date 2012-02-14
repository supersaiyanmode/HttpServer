#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctype.h>

#include "HTTPRequest.h"
#include "SocketUtils.h"

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

HTTPRequest::HTTPRequest(){
    
}

std::string HTTPRequest::getUrl(){
    return url;
}

const std::map<std::string, std::string> HTTPRequest::getHeaders(){
    return headers;
}

std::string HTTPRequest::getContent(){
    return body;
}

HTTPRequestMethod HTTPRequest::getMethod(){
    return method;
}

void HTTPRequest::display(){
    std::cout<<"HTTPRequest Display:\n";
    std::cout<<(method==GET?"GET":method==POST?"POST":method==DELETE?"DELETE":"PUT");
    std::cout<<" "<<url<<" "<<version<<std::endl;
    std::cout<<std::endl;
    for (std::map<std::string,std::string>::iterator it=headers.begin(); it!=headers.end(); it++){
        std::cout<<it->first<<" : "<<it->second<<std::endl;
    }
    std::cout<<std::endl;
    std::cout<<body;
    std::cout<<std::endl;
}

HTTPRequest HTTPRequest::readFromSocket(int socket){
    HTTPRequest ret;
    
    std::string requestLine;
    std::map<std::string, std::string> headers;
    std::string body;
    
    //request line..
    std::string line;
    std::vector<std::string> headersVector;
    bool good = true;
    if (!readLine(socket,requestLine))
        throw "Can't read Request Line!";
    std::cout<<"$: "<<line;
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

    ret.url = urlStr;
    ret.version = versionStr;
    
    while(readLine(socket,line)){
        if (line == "")
            break;
        headersVector.push_back(line);
    }
    
    ret.headers = makeHeaders(headersVector);
    if (ret.method == POST){
        std::stringstream ss(ret.headers["content-type"]);
        int length;
        ss>>length;
        ret.body.resize(length);
        if (!readBytes(socket, &(ret.body[0]), length))
            throw "Unable to read fully!";
    }
    return ret;
}