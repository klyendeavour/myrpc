#pragma once
#include "google/protobuf/service.h"
#include "muduo/net/TcpConnection.h"
#include "unordered_map"
#include "log.h"
//用于专门发布RPC服务的网络类
class MyrpcProvider{
    public:
        //供myrpc框架使用者发布远程调用服务，把服务对象发布到myrpc结点上
        void notifyService(google::protobuf::Service* service);
        //启动rpc服务发布结点，run以后进程进入阻塞状态，等待远程的rpc调用请求
        void run();
        MyrpcProvider();
    private:
        //RPC服务对象的详细信息
        struct ServiceInfo{
            //提供rpc服务的对象
            google::protobuf::Service* service;
            //服务提供的方法
            std::unordered_map<std::string,const google::protobuf::MethodDescriptor*>serviceMethodMap;
        };
        //根据rpc服务对象的名字找到rpc服务对象
        std::unordered_map<std::string,MyrpcProvider::ServiceInfo> serviceMap;

        //客户连接事件回调
        void onConnectCallback(const muduo::net::TcpConnectionPtr&);
        //客户读写事件回调
        void onMessageCallback(const muduo::net::TcpConnectionPtr&,muduo::net::Buffer*,muduo::Timestamp );
        //客户的请求处理完毕后的回调函数
        void onDealResonse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*);
};