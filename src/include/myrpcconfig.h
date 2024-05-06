#pragma once

#include <unordered_map>
#include "string"
class MyrpcConfig
{
private:
    //存储配置信息
    std::unordered_map<std::string,std::string> m_config;
    //去掉字符串前后空格
    void trim(std::string&);
    /* data */
public:
    //加载配置文件并解析
    void loadConfigFile(const char * file);
    //获取配置信息
    std::string  getConfig(const std::string& key);
};


