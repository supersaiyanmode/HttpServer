#include <iostream>
#include <sstream>
#include <algorithm>
#include <ctype.h>
#include <stdio.h>

#include "HTTPRequest.h"
#include "SocketUtils.h"

const std::string CRLF = "\r\n";
std::pair<int, std::string> httpStatusCodeData[] = {
    std::make_pair(200,"OK"),
    std::make_pair(302,"Found"),
    std::make_pair(404,"Not Found"),
    std::make_pair(500,"Internal Server Error")
};

std::map<int, std::string> httpStatusCodes(httpStatusCodeData,
    httpStatusCodeData + sizeof(httpStatusCodeData)/sizeof(httpStatusCodeData[0]));

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

std::string encodeURIComponent(const std::string &s){
    //RFC 3986 section 2.3 Unreserved Characters (January 2005)
    const std::string unreserved = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

    std::string escaped="";
    for(size_t i=0; i<s.length(); i++){
        if (unreserved.find_first_of(s[i]) != std::string::npos){
            escaped.push_back(s[i]);
        }else{
            escaped.append("%");
            char buf[3];
            sprintf(buf, "%.2X", s[i]);
            escaped.append(buf);
        }
    }
    return escaped;
}

std::string decodeURIComponent(const std::string& s){
    static const std::string hexChars = "0123456789ABCDEF";
    std::string ret="";
    for(size_t i=0, len=s.length(); i<len; i++){
        if (s[i] == '%'){
            if (i >= len-2){
                return ret;
            }
            size_t pos1 = hexChars.find_first_of(::toupper(s[i+1]));
            size_t pos2 = hexChars.find_first_of(::toupper(s[i+2]));
            if (pos1 == std::string::npos || pos2 == std::string::npos)
                continue; //invalid char here, ignore and continue..
            ret.append(1,(char)(pos1*16 + pos2));
            i+=2;
        }else{
            ret.append(1,s[i]);
        }
    }
    return ret;
}

std::map<std::string, std::string> decodeParams(std::string queryString){
    std::map<std::string, std::string> ret;
    size_t posAmp;
    while (1){
        posAmp = queryString.find_first_of('&');
        size_t posEqual;
        std::string kvPairStr;
        if (posAmp == std::string::npos){
            kvPairStr = queryString;
            posEqual = kvPairStr.find_first_of('=');
            if (posEqual == std::string::npos)
                return ret;
            ret[kvPairStr.substr(0,posEqual)] = decodeURIComponent(kvPairStr.substr(posEqual+1));
            return ret;
        }
        kvPairStr = queryString.substr(0,posAmp);
        posEqual = kvPairStr.find_first_of('=');
        if (posEqual == std::string::npos)
            continue; //error
        ret[kvPairStr.substr(0,posEqual)] = decodeURIComponent(kvPairStr.substr(posEqual+1));
        queryString = queryString.substr(posAmp+1);
    }
    return ret;
}


std::string encodeParams(const std::map<std::string, std::string>& param){
    std::string ret = "";
    for (std::map<std::string, std::string>::const_iterator it=param.begin(); it!=param.end(); it++){
        ret += it->first + "=" + encodeURIComponent(it->second);
    }
    return ret;
}

std::ostream& operator<<(std::ostream& o, const std::map<std::string, std::string>& map){
    for (std::map<std::string, std::string>::const_iterator it=map.begin(); it!= map.end(); it++){
        o<<it->first<<" : "<<it->second<<std::endl;
    }
    return o;
}

//##############HTTPMessage#######################
const std::map<std::string, std::string>& HTTPMessage::getHeaders(){
    return headers;
}



std::string HTTPMessage::getContent(){
    return body;
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

    //separate GET params
    size_t posQmark= urlStr.find_first_of('?');
    if (posQmark == std::string::npos){
        ret.url = urlStr;
    }else{
        ret.url = urlStr.substr(0,posQmark);
        
        if (posQmark < urlStr.length()-1){ //ignore the URL with '?' at the end..
            std::string queryString = urlStr.substr(posQmark+1),param;
            ret.getParam = decodeParams(queryString);
        }
    }
    ret.version = versionStr;
    
    while(readLine(socket,line)){
        if (line == "")
            break;
        headersVector.push_back(line);
    }
    
    ret.headers = makeHeaders(headersVector);
    
    //make cookieSet
    if (ret.headers.find("cookie") != ret.headers.end()){
        ret.cookies = CookieSet::fromHeaderString(ret.headers["cookie"]);
    }

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

std::string HTTPRequest::getUrl(){
    return url;
}
HTTPRequestMethod HTTPRequest::getMethod(){
    return method;
}
std::string HTTPRequest::getMethodStr(){ //TODO: Fix this quick and dirty stuff..
    return (method==GET?"GET":method==POST?"POST":method==DELETE?"DELETE":"PUT");
}

const std::map<std::string, std::string> HTTPRequest::getParams(){
    return getParam;
}
const std::map<std::string, std::string> HTTPRequest::postParams(){
    return postParam;
}


std::string HTTPRequest::str(std::string prepend){
    std::string ret = prepend + getMethodStr() + " " + getUrl() + " " + version + CRLF;
    for (std::map<std::string,std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
        ret += prepend + it->first + ": " + it->second + CRLF;
    ret += CRLF;
    ret += prepend + prepend + body;
    return ret;
}

//####################HTTPResponse###################
HTTPResponse::HTTPResponse(){
    version = "HTTP/1.0";
    setStatusCode(200);
}
void HTTPResponse::setStatusCode(int c){
    statusMsg = httpStatusCodes[c];
    code = c;
    std::stringstream ss;
    ss<<c;
    codeStr = ss.str();
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

std::string HTTPResponse::str(std::string prepend){
    std::string ret = prepend + version + " " + codeStr + " " + statusMsg + CRLF;
    
    //set header for cookies
    std::string cookieStr = cookies.str();
    if (cookieStr != "")
        headers["Set-Cookie"] = cookieStr;
    
    for (std::map<std::string,std::string>::iterator it=headers.begin(); it!=headers.end(); it++)
        ret += prepend + it->first + ": " + it->second + CRLF;
    ret += CRLF;
    ret += prepend + prepend+body;
    return ret;
}

void HTTPResponse::redirect(const std::string &url){
    setContent("");
    setStatusCode(302);
    setHeader("Location",url);
}