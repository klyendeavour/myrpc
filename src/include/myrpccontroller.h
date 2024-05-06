#pragma once
#include "google/protobuf/service.h"

//设置和返回处理数据状态
class MyrpcController:public google::protobuf::RpcController{

public:
    MyrpcController():errText(""),failed(false){}
    //重置
     void Reset()override;
    //执行是否出错
     bool Failed()const override;
    //获取错误信息
    std::string ErrorText()const override;
    //置错
     void SetFailed(const std::string& reason)override;



    //未使用这些功能，以下方法不再实现
     void StartCancel()override;

     bool IsCanceled()const override;
     
     void NotifyOnCancel(google::protobuf::Closure* callback)override;


private:
    std::string errText;
    bool failed;

};