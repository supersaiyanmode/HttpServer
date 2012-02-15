#include "CookieManager.h"

std::string formatTimeStamp(time_t t){
    //Wdy, DD-Mon-YYYY HH:MM:SS GMT
    tm tm_struct = *::gmtime(&t);
    char buf[100];
    strftime(buf,100,"%A, %d-%b-%y %H:%M:%S GMT", &tm);
    return buf;
}

Cookie(const std::string& k):key(k){
    
}

std::string Cookie::str(){
    return key + "=" + value + "; Path=" + path + "; Expires=" + formatTimeStamp(expires);
}
void Cookie::expire(){
    expires = 0;
}
void Cookie::setPath(const std::string& p){
    path = p;
}
    
void Cookie::setExpiration(time_t t){
    expires = t
}
void Cookie::setValue(const std::string& v){
    value = v;
}


//##########COOKIE SET#################
CookieSet::CookieSet(const std::string& str){
    //read Cookie from the string.. sent by client thru Cookie: Header..
}
const Cookie& CookieSet::operator[](const std::string& key) const{
    if (cookies.find(key) == cookies.end())
        return Cookie();
    return cookies.find(key)->second;
}
Cookie& Cookie::operator[](const std::string& key){
    return cookies[key];
}
    
std::string Cookie::str() const{
    std::string ret = "";
    for (std::map<std::string, Cookie>::iterator it=cookies.begin(); it!=cookies.end(); it++){
        ret += it->first.str() + ", ";
    }
    ret = ret.substr(0,ret.length()-2); //remove last ", "
    return ret;
}