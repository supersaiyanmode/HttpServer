#ifndef COOKIE_MANAGER_H
#define COOKIE_MANAGER_H
#include <string>
#include <map>
#include <time.h>

class Cookie{
    std::string key;
    std::string value;
    std::string path;
    ::time_t expires;
public:
    Cookie(const std::string&);
    std::string str();
    void expire();
    void setPath(const std::string&);
    void setExpiration(::time_t);
    void setValue(const std::string&);
};

class CookieSet{
    std::map<std::string, Cookie> cookies;
public:
    CookieSet(const std::string&);
    const Cookie& operator[](const std::string&) const;
    Cookie& operator[](const std::string&);
    
    std::string str() const;
}

#endif