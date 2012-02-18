#include "FileLister.h"
#include <dirent.h>
#include <algorithm>
#include <iostream>

const char SEP = '/';

FileLister::FileLister(const std::string& path):basePath(path){
    if (basePath[basePath.length()-1] != SEP)
        basePath.append(1,SEP);
    list();
}

std::string FileLister::getCurrentPath(){
    return basePath + getPath();
}

int FileLister::list(){
    directories = files = std::vector<std::string>();
    ::DIR *dir = opendir(getCurrentPath().c_str());
    std::cout<<"Listing Path: "<<getCurrentPath()<<std::endl;
    if (dir != NULL) {
        ::dirent *ent;
        while ((ent = readdir (dir)) != NULL) {
            std::string fileName(ent->d_name);
            if (fileName == "." || fileName == "..")
                continue;
            if (ent->d_type == ::DT_REG)
                files.push_back(ent->d_name);
            else if (ent->d_type == ::DT_DIR)
                directories.push_back(ent->d_name + std::string(1,SEP));
        }
        closedir (dir);
        std::sort(files.begin(), files.end());
        std::sort(directories.begin(), directories.end());
        files.insert(files.begin(), directories.begin(), directories.end());
        return 1;
    } else {
        std::cout<<"Can't list!\n";
        return 0;
    }
}

int FileLister::enterDir(const std::string& dirName){
    //dirName must end with SEP
    if (dirName[dirName.length()-1] != SEP){
        std::cout<<"Doesn't end with SEP\n";
        return 0;
    }
    //must exist .. else return; invalidity holds for "." and ".." too
    if (std::find(files.begin(), files.end(), dirName)==files.end()){
        std::cout<<"Doesn't exist\n";
        return 0;
    }
    FileLister backup(*this);
    dirStack.push(dirName);
    if (!list()){
        *this = backup;
        return 0;
    }
    return 1;
}

int FileLister::exitDir(){
    if (dirStack.empty())
        return 0;
    dirStack.pop();
    return list();
}

int FileLister::setPath(std::string path){
    if (path[path.length()-1] != SEP)
        path.append(1,SEP);
    FileLister backup(*this);
    dirStack = std::stack<std::string>();
    list();
    do{
        size_t posSlash = path.find_first_of(SEP);
        std::string dirName = path.substr(0, posSlash+1);
        if (!enterDir(dirName)){
            *this = backup;
            return 0;
        }
        if (posSlash == path.length()-1)
            break;
        path = path.substr(posSlash+1);
    }while (1);
    return 1;
}
std::string FileLister::getPath(){
    std::string ret;
    std::stack<std::string> tempDirStack(dirStack);
    while (!tempDirStack.empty()){
        ret = tempDirStack.top() + ret;
        tempDirStack.pop();
    }
    return ret;
}

const std::vector<std::string>& FileLister::get(){
    return files;
}

#if 0
int main(){
    FileLister fl("/home/thrustmaster");
    std::string cmd,param;
    do {
        std::cout<<"$: ";
        std::cin>>cmd;
        if (cmd == "ls"){
            std::vector<std::string> v(fl.get());
            for (std::vector<std::string>::iterator it=v.begin(); it!=v.end(); it++){
                std::cout<<*it<<std::endl;
            }
        }else if (cmd == "cd"){
            std::cin>>param;
            std::cout<<(fl.enterDir(param)?"success":"error")<<std::endl;
        }else if (cmd == "cd.."){
            std::cout<<(fl.exitDir()?"success":"error")<<std::endl;
        }else if (cmd == "set"){
            std::cin>>param;
            std::cout<<(fl.setPath(param)?"success":"error")<<std::endl;
        }
            
    }while (cmd != "quit");
}

#endif