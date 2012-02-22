#include "FileLister.h"
#include <dirent.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>

const char SEP = '/';

std::map<std::string, std::string> MIME_MAP;



FileLister::FileLister(const std::string& path):basePath(path){
    if (!MIME_MAP.size()){
        //populate the mime table
        std::ifstream in("mimetypes");
        std::string line;
        while (std::getline(in, line)){
            std::stringstream ss(line);
            std::string value,key;
            ss>>value;
            while (ss>>key)
                MIME_MAP[key] = value;
        }
        in.close();
        MIME_MAP[""] = "application/octet-stream";
    }
    if (basePath[basePath.length()-1] != SEP)
        basePath.append(1,SEP);
    list();
}

std::string FileLister::getCurrentPath(){
    return basePath + getPath();
}

int FileLister::list(){
    directories = files = std::vector<std::string>();
    std::string curPath(getCurrentPath());
    ::DIR *dir = ::opendir(curPath.c_str());
    std::vector<std::string> fileList;
    if (dir != NULL) {
        ::dirent *ent;
        while ((ent = readdir (dir)) != NULL) {
            std::string fileName(ent->d_name);
            if (fileName == "." || fileName == "..")
                continue;
            fileList.push_back(ent->d_name);
        }
        closedir (dir);
        for (std::vector<std::string>::iterator it=fileList.begin();
                    it!=fileList.end(); it++){
            ::DIR *folderCheck;
            if ((folderCheck = ::opendir((curPath + *it).c_str())))
                directories.push_back(*it + std::string(1,SEP));
            else
                files.push_back(*it);
            closedir(folderCheck);
        }
        std::sort(files.begin(), files.end());
        std::sort(directories.begin(), directories.end());
        files.insert(files.begin(), directories.begin(), directories.end());
        return 1;
    } else {
        std::cout<<"Can't list!\n";
        return 0;
    }
}

int FileLister::enterDir(std::string dirName){
    //dirName must end with SEP
    if (dirName[dirName.length()-1] != SEP){
        dirName.append(1,SEP);
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

bool FileLister::getFileContent(const std::string& fileName,std::string& retStr,
                std::string& mime){
    if (fileName[fileName.length()-1] == SEP){
        retStr = "filename ends with " + std::string(1,SEP);
        return false;
    }
    if (std::find(files.begin(), files.end(), fileName)==files.end()){
        retStr = "File not in the list of files in cur dir..";
        return false;
    }
    std::ifstream in(fileName.c_str(), std::ios::in|std::ios::binary);
    if (!in.good()){
        retStr = "Grr.. file doesn't exist";
        return false;
    }
    std::stringstream ss;
    ss<<in.rdbuf();
    in.close();
    retStr = ss.str();
    std::string extension = "";
    size_t pos;
    if ((pos =fileName.find_last_of(".")) != std::string::npos)
        extension = fileName.substr(pos);

    mime = MIME_MAP[extension];
    return true;
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