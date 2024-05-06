#include "myrpcapplication.h"
#include "iostream"
#include "string"
#include "unistd.h"

void applicationArgHelp(){
    std::cout<<"format: command -i <configfilename> "<<std::endl;
}

//懒汉式单例
MyrpcApplication& MyrpcApplication::getInstance(){
            static MyrpcApplication rpcApplication;
            return rpcApplication;
}   
//加载框架的配置信息
void MyrpcApplication::init(int argc,char** argv){
    if(argc < 2){
        applicationArgHelp();
        exit(EXIT_FAILURE); 
    }
 
    std::string config_file_name;
    int c;
    //解析参数，得到配置文件
    while((c = getopt(argc,argv,"i:")) != -1){
        switch (c)
        {
        case 'i':
            config_file_name = optarg;
            break;
        case '?':
            applicationArgHelp();
        case ':':
            applicationArgHelp();
            break;
        default:
            break;
        }
    }
    
    //没有获取到配置文件
    if(!config_file_name.size()){
        applicationArgHelp();
        exit(EXIT_FAILURE);
    }


    m_myrpcConfig.loadConfigFile(config_file_name.c_str());

}


MyrpcConfig& MyrpcApplication::getMyrpcConfig(){
    return m_myrpcConfig;
}