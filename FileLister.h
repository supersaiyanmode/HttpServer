#ifndef FILELISTER_H
#define FILELISTER_H
#include <string>
#include <vector>
#include <stack>

class FileLister{
    std::string basePath;
    std::vector<std::string> files,directories;
    std::stack<std::string> dirStack; //avoid open("..") to move up..
    int list();
    std::string getCurrentPath();
public:
    FileLister(const std::string&);
    int enterDir(std::string);
    int exitDir();
    const std::vector<std::string>& get();
    int setPath(std::string);
    std::string getPath();
};

#endif