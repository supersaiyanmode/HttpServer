#ifndef COOKIE_MANAGER_H
#define COOKIE_MANAGER_H
#include <string>
#include <map>
#include <time.h>

class Cookie{
    std::string key_;
    std::string value_;
    std::string path;
    ::time_t expires;
    bool expirationSet;
public:
    Cookie();
    Cookie(const std::string&,const std::string&);
    std::string operator=(const std::string&);
    std::string str() const;
    std::string value() const;
    std::string key() const;
    void expire();
    void setPath(const std::string&);
    void setExpiration(::time_t);
    void setValue(const std::string&);
};

class CookieSet{
    std::map<std::string, Cookie> cookies;
public:
    Cookie& operator[](const std::string&);
    
    std::string str() const;
    
    static CookieSet fromHeaderString(const std::string&);
};
#endif