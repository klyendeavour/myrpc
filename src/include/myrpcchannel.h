#pragma once
#include "google/protobuf/service.h"

class MyrpcChannel:public google::protobuf::RpcChannel
{
private:
    /* data */
public:
    //用户调用的方法，都会到这里进行请求参数的反序列化和网络同步发送
  void CallMethod(const google::protobuf::MethodDescriptor* method,
                        google::protobuf:: RpcController* controller, 
                        const google::protobuf:: Message* request,
                        google::protobuf::Message* response, 
                        google::protobuf::Closure* done);
};


