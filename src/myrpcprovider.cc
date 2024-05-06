#include "myrpcprovider.h"
#include "myrpcapplication.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpServer.h"
#include "string"
#include "functional"
#include "google/protobuf/descriptor.h"
#include "header.pb.h"
#include "zookeeperutil.h"
MyrpcConfig MyrpcApplication::m_myrpcConfig;

void MyrpcProvider::notifyService(google::protobuf::Service* service){ 
        ServiceInfo serviceinfo;
        serviceinfo.service = service;
        //服务对象的描述
        const ::google::protobuf::ServiceDescriptor* serviceDescriptor = service->GetDescriptor();
       //服务对象的名字
        std::string serviceName = serviceDescriptor->name();
        
        //服务对象的方法数量
        int methodNum = serviceDescriptor->method_count(); 
        //指向服务方法
        const google::protobuf::MethodDescriptor*method;
        //服务方法的名字
        std::string methodName;
        for(int i = 0;i < methodNum;i++){
            method = serviceDescriptor->method(i);
            methodName = method->name();
            serviceinfo.serviceMethodMap.insert({std::move(methodName),std::move(method)});
        }
        this->serviceMap.insert({std::move(serviceName),std::move(serviceinfo)});
}
        
void MyrpcProvider::run(){
    //epoll
    muduo::net::EventLoop loop;
    //获取rpc服务器的ip和端口
    MyrpcConfig rpcConfig = MyrpcApplication::getMyrpcConfig();
    std::string key1 = "myrpc_server_ip";
    std::string key2 = "myrpc_server_port";
    std::string ip = rpcConfig.getConfig(key1);
    int port =  atoi(rpcConfig.getConfig(key2).c_str());
    muduo::net::InetAddress addr(ip,port);

    muduo::net::TcpServer server(&loop,addr,"rpcserver");//server绑定ip和端口

    //设置回调 和线程数量
    server.setThreadNum(2);    
    server.setConnectionCallback(std::bind(&MyrpcProvider::onConnectCallback,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&MyrpcProvider::onMessageCallback,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));


    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点    method_name为临时性节点
    for (auto &sp : this->serviceMap) 
    {
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.serviceMethodMap)
        {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    // rpc服务器要启动
    LOG_INFO("service start !!!  ip: %s  port: %d",ip.c_str(),port);
    std::cout << " service start !!!  ip:" << ip << " port:" << port << std::endl;


    server.start();//listenfp -> epoll
    loop.loop();//阻塞等待监听事件发生
}


void MyrpcProvider::onConnectCallback(const muduo::net::TcpConnectionPtr& con){
    
    if(con->disconnected()){
        con->shutdown();
    }
}

//拿到需要提供的远程服务对象的名字和方法名，当然这要经过字符流的反序列化，然后调用对应服务提供的方法，最后注册处理完成后的回调
//调用方和服务方规定好，前四个字节对应header的大小   headr中包含服务对象的名字和方法，还有方法参数的大小，防止tcp粘包
void MyrpcProvider::onMessageCallback(const muduo::net::TcpConnectionPtr&con,muduo::net::Buffer*buffer,muduo::Timestamp t){
    
    //取到缓冲的字符流
    std::string strbuf = buffer->retrieveAllAsString();
    //获取前四个字节得到头的长度
    uint headerLen = 0;
    strbuf.copy((char*)&headerLen,4,0);


    //对header反序列化
    std::string header_str = strbuf.substr(4,headerLen);
    header_proto::Header header;
    if(!header.ParseFromString(header_str)){
        LOG_ERR("header反序列化失败");
        return;
    }
    //获取参数的序列化字符流
    std::string arg_str = strbuf.substr(4+headerLen,header.arg_size());
    
    auto it = serviceMap.find(header.service_name().c_str());
    
    google::protobuf::Service* service = nullptr;
    const google::protobuf::MethodDescriptor* method = nullptr;
    if(it == serviceMap.end()){//没有这个服务对象
        LOG_INFO("not find service : %s",header.service_name().c_str());
        return;
    }else{
        LOG_INFO("visit service %s",header.service_name().c_str());
        service = (it->second).service;
    }
    auto it_method = (it->second).serviceMethodMap.find(header.method_name());

    if(it_method == (it->second).serviceMethodMap.end()){//服务对象没头这个方法
        LOG_INFO("not find service %s's method : %s",header.service_name().c_str(),header.method_name().c_str());
        return;
    }else{
        LOG_INFO("visit service: %s's mothed: %s",header.service_name().c_str(),header.method_name().c_str());
        method = it_method->second;
    }   
    //现在提供服务的对象和方法都拿到了


    //根据对象和方法new一个请求参数
    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    //请求参数反序列化
    if(!request->ParseFromString(arg_str)){
        LOG_ERR("request_arg get fail,cant Parse");
        return;
    }


    //根据对象和方法new一个响应参数
    google::protobuf::Message* response = service->GetResponsePrototype(method).New();
    //注册处理完毕后的响应动作
    google::protobuf::Closure*done = google::protobuf::NewCallback<MyrpcProvider,
                                                                const muduo::net::TcpConnectionPtr&,
                                                                google::protobuf::Message*>(this,
                                                                &MyrpcProvider::onDealResonse,
                                                                con,response);
    //调用服务对应的方法
    service->CallMethod(method,nullptr,request,response,done);

}
void MyrpcProvider::onDealResonse(const muduo::net::TcpConnectionPtr& con,google::protobuf::Message* response){
        //响应参数序列化
        std::string responseStr;
        if(!response->SerializeToString(&responseStr)){
            LOG_ERR("response serialize failure");
            
        }else{
            con->send(responseStr);
        }
        con->shutdown();//主动关闭连接，提供短连接

}

MyrpcProvider::MyrpcProvider(){
        //开启日志
        Log::getInstance().init();
}