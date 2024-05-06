#pragma once
#include "syncqueue.h"
#define LOG_INFO(format,...)\
            do{\
                Log::getInstance().setLogLevel(LogLevel::INFO);\
                char buf[1024];\
                sprintf(buf,format,##__VA_ARGS__);\
                Log::getInstance().writeLog(buf);\
            }\
            while(0);
#define LOG_ERR(format,...)\
            do{\
                Log::getInstance().setLogLevel(LogLevel::ERROR);\
                char buf[1024];\
                sprintf(buf,format,##__VA_ARGS__);\
                Log::getInstance().writeLog(buf);\
            }\
            while(0);


enum LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};



class  Log
{

private:
    Log() = default;
    Log(const Log&)=delete;
    Log(Log&&)=delete;
    int m_logLavel;
    SyncQueue<std::string> m_queue;
    //日志文件的日期
    std::string m_time;
    //日志文件是否关闭
    bool isClosed;
    //文件指针
    FILE* fp;
public:  
    //日志模块初始化，开启日志写线程
    void init();
    //设置日志级别
    void setLogLevel(int level);
    //写日志，把信息放入队列中
    void writeLog(std::string);
    //懒汉单例
   static Log& getInstance();
};






