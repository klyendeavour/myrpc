#include "myrpcconfig.h"
#include "string"
#include "iostream"

void MyrpcConfig::loadConfigFile(const char * file){
    FILE* fp = fopen(file,"r");
    if(fp == nullptr){
        std::cout<<"no such file"<<std::endl;
        exit(EXIT_FAILURE);
    }
    char buf[512];
    std::string key;
    std::string value;
    while(!feof(fp)){
        //读一行
        fgets(buf,512,fp);
        //1.注释 2. key=value 3.空白行
        std::string str(buf);
        //除去行首空格，除去行尾空格
        trim(str);
        //跳过注释行 和 空白行
        if(!str.size() || str[0] == '#'){
            continue;
        }
        //除去结尾 '\n'
        int index = str.find('\n');
        if(index != -1){
           str =  str.substr(0,index);
        }
         index = str.find('=');
         key = str.substr(0,index);
         value = str.substr(index+1,str.size() - index + 1);
         trim(key);
         trim(value);
        if(index != -1){
            this->m_config.insert(std::pair<std::string,std::string>(std::move(key),std::move(value)));
        }

       
    }
}

std::string MyrpcConfig::getConfig(const std::string& key){
        auto i = this->m_config.find(key);
        //没找到返回 空串
        if(i == this->m_config.end()){
            return "";
        }
        return i->second;
}

void MyrpcConfig::trim(std::string&str){
    //除去行首空格
        int index = 0;
        index = str.find_first_not_of(' ');
        if(index != -1){
          str =  str.substr(index,str.size() - index);
        }
        //除去行尾空格
        index = str.find_last_not_of(' ');
        if(index != -1){
           str =  str.substr(0,index + 1);
        }

}