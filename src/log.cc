#include "log.h"
#include "thread"
#include "time.h"
#include "string.h"
#include "iostream"
Log& Log::getInstance(){
        static Log log;
        return log;
}

void Log::init(){
    isClosed = true;
    m_time = "";
    fp = nullptr;
    std::thread logThread([&,this](){
        while(1){ //文件名：年月日-log.txt       
            //获取系统当前时间
            time_t now = time(nullptr);
            tm* nowLocal = localtime(&now);
            char buf_time[1024];
            sprintf(buf_time,"%d-%d-%d-log.txt",nowLocal->tm_year+1900,nowLocal->tm_mon+1,nowLocal->tm_mday);
            //根据日期判断是否打开新的日志文件
            if(strcmp(m_time.c_str(),buf_time) != 0){
                    //没有关闭就关闭
                    if(!isClosed){
                        fclose(fp);
                        fp = nullptr;
                    }
                    this->m_time = buf_time;
            }
           
            //从队列取出信息
            std::string logContent = m_queue.syncPop();
            
             //打开文件
            if(this->isClosed){
                this->fp = fopen(this->m_time.c_str(),"a+");
                this->isClosed = false; //打开
            }
           
           //构造记录信息的格式   ： 时-分-秒  [Info]  信息
            char buf_log_pri[512];
            sprintf(buf_log_pri,"%d-%d-%d  ",nowLocal->tm_hour,nowLocal->tm_min,nowLocal->tm_sec);
            logContent.insert(0,buf_log_pri);
            logContent.append("\n");
            fputs(logContent.c_str(),this->fp);
            //队列没有信息了就关闭文件
            if(m_queue.syncSize() == 0){
                fclose(this->fp);
                fp = nullptr;
                 this->isClosed = true;
            }  
        }
    });
    //线程分离
    logThread.detach();

}
void Log::setLogLevel(int level){
    
    m_logLavel = level;
    
}
void Log::writeLog(std::string str){
    //填入日志级别
    std::string level = this->m_logLavel == LogLevel::DEBUG?" [debug] " :
                                this->m_logLavel == LogLevel::INFO?" [info] ":
                                this->m_logLavel == LogLevel::WARNING?" [warning] ":
                                this->m_logLavel == LogLevel::ERROR?" [error] ":" [critical] ";
    str.insert(0,level.c_str());
    m_queue.syncPush(str);
}