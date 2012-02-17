#include "CookieManager.h"
#include <iostream>
std::string formatTimeStamp(time_t t){
    //Wdy, DD-Mon-YYYY HH:MM:SS GMT
    tm tm_struct = *::gmtime(&t);
    char buf[100];
    ::strftime(buf,100,"%A, %d-%b-%y %H:%M:%S GMT", &tm_struct);
    return buf;
}

Cookie::Cookie():expirationSet(false){}

Cookie::Cookie(const std::string& k, const std::string& v):
                    key_(k),value_(v),path("/"),expirationSet(false){
}

std::string Cookie::operator=(const std::string& str){
    return value_ = str;
}


std::string Cookie::value() const{
    return value_;
}
std::string Cookie::str() const{
    if (key_ == "")
        return "";
    return (key_!=""? key_ + "=" + value_:"") +
           "; Path=" + path +
           (expirationSet?"; Expires=" + formatTimeStamp(expires) : "");
}
void Cookie::expire(){
    expirationSet = true;
    expires = 0;
}
void Cookie::setPath(const std::string& p){
    path = p;
}
    
void Cookie::setExpiration(time_t t){
    expirationSet = true;
    expires = t;
}
void Cookie::setValue(const std::string& v){
    value_ = v;
}


//##########COOKIE SET#################
Cookie& CookieSet::operator[](const std::string& key){
    cookies[key] = Cookie(key,"");
    return cookies[key];
}
    
std::string CookieSet::str() const{
    std::string ret;
    for (std::map<std::string, Cookie>::const_iterator it=cookies.begin();
                    it!=cookies.end(); it++){
        ret += it->second.str() + ", ";
    }
    ret = ret.substr(0,ret.length()-2); //remove last ", "
    return ret;
}

CookieSet CookieSet::fromHeaderString(const std::string& str){
    return CookieSet();
}