#pragma once
#include "myrpcconfig.h"
#include "myrpcchannel.h"
#include "myrpccontroller.h"
//myrpc框架的基础类，供使用者使用该框架
class MyrpcApplication{
    private:
        MyrpcApplication(){};
        MyrpcApplication(MyrpcApplication&)=delete;
        MyrpcApplication(MyrpcApplication&&)=delete;
    public:
        //懒汉式单例
        static MyrpcApplication& getInstance(); 
        //加载框架的配置信息
        static void init(int argc,char** argv);
        //解析配置文件的对象,完成后保存配置信息
        static MyrpcConfig m_myrpcConfig;
        //获取解析配置文件的对象
        static MyrpcConfig& getMyrpcConfig();
};


