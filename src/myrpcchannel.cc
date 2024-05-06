#include "myrpcchannel.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "header.pb.h"
#include "sys/socket.h"
#include "unistd.h"
#include "arpa/inet.h"
#include <sys/types.h>
#include "myrpcapplication.h"
#include "myrpcconfig.h"
#include "zookeeperutil.h"
#include "memory"
#include "functional"
 void MyrpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                        google::protobuf:: RpcController* controller, 
                        const google::protobuf:: Message* request,
                        google::protobuf::Message* response, 
                        google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* serviceDescriptor =  method->service();
    //获取服务对象的名字
    std::string service_name = serviceDescriptor->name();
    //获取方法的名字
    std::string method_name = method->name();

    //发送格式:: header_size + header_str(service_name + method_name + arg_size) + arg_str

    //请求参数序列化
    std::string arg_str;

    if(!request->SerializeToString(&arg_str)){
        controller->SetFailed("arg serialize failed");
        return;
    }

    uint32_t arg_size = arg_str.size();
    //header序列化
    std::string header_str;
    header_proto::Header header;
    header.set_service_name(service_name);
    header.set_method_name(method_name);
    header.set_arg_size(arg_size);

    if(!header.SerializeToString(&header_str)){
        controller->SetFailed("header serialize failed");
        return;
    }

    //header序列化字符流的大小
    uint32_t header_size = header_str.size();

    //最终发送的字符流
    std::string send_str;
    //规定前send_str的前4个字节存放header_size，即存放其对应数字的二进制
    send_str.insert(0,std::string((char*)&header_size,4));

    send_str += header_str;
    send_str += arg_str;


    //开始网络发送
    int fd = socket(AF_INET,SOCK_STREAM,0);
    
    //使用智能指针管理socket,并指定删除器
    std::unique_ptr<int,std::function<void(int*)>> fd_ptr(new int(fd),[](int * fd){
        if(fd != nullptr){
            close(*fd);
            delete fd;
            fd = nullptr;
        }
    });

    

    if(fd == -1){
        controller->SetFailed("client create socket failed ! errno:");
        return;
    }

 
     // 读取配置文件rpcserver的信息
    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    // rpc调用方想调用service_name的method_name服务，需要查询zk上该服务所在的host信息
    ZkClient zkCli;
    zkCli.Start();
    //  /UserServiceRpc/Login
    std::string method_path = "/" + service_name + "/" + method_name;
    // 127.0.0.1:8000
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "")
    {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx+1, host_data.size()-idx).c_str()); 

    //绑定服务器ip 和端口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
   

    //建立连接
   if (connect(fd,(sockaddr*)&addr,sizeof(addr)) == -1){    
        std::string errStr = "failed to connect, errno:" + errno;
        controller->SetFailed(errStr);
        return;
   }

    //开始发送
   if(send(fd,send_str.c_str(),send_str.size(),0) == -1){
         std::string errStr = "failed to send ! errno" + errno;
         controller->SetFailed(errStr);
        return;
   }

    //同步接收数据
    char recv_buf[1024];
    int recv_len = 0;
    if((recv_len = recv(fd,recv_buf,1024,0)) == -1){
        if(errno != EINTR){
            std::string errStr = "failed to recv! errno" + errno;
            controller->SetFailed(errStr);
            return;
        }
    }

  //  std::string response_str(recv_buf,recv_len);   错误 字符流处遇到0 就截断了buf ，造成response数据错误
   if(!response->ParseFromArray(recv_buf,recv_len)){    
        std::string errStr = "failed to parseResponse" + errno;
        controller->SetFailed(errStr);
        return;
   }
   
}
