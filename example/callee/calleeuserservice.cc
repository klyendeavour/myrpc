//远程服务的提供者，提供登录服务
#include <iostream>
#include "string"
#include "user.pb.h"
#include "myrpcapplication.h"
#include "myrpcprovider.h"

class UserService:public user_proto::UserServiceRpc
{
private:
    /* data */
public:

    //过程: caller->Login(LoginRequest)序列化->muduo->反序列化
    //      callee->调用服务方法
   //发布远程服务方法
  virtual void Login(::google::protobuf::RpcController* controller,
                       const ::user_proto::LoginRequest* request,
                       ::user_proto::LoginResponse* response,
                       ::google::protobuf::Closure* done){
                    
                

                std::string name =  request->name(); 
                std::string pwd = request->pwd();

                  //调用本地方法执行业务处理
               bool dealResult =  Login(name,pwd);

               //封装返回结果response
               response->set_success(dealResult);
                user_proto::ResponseState*state = response->mutable_state();
                state->set_errcode(0);
                state->set_errmsg("");

                //回调操作     框架完成响应参数序列化和网络发送
                done->Run();
}

    //真正执行业务的本地方法
    bool  Login(std::string name,std::string pwd){
        std::cout<<"name:"<<name<<std::endl;
        std::cout<<"pwd:"<<pwd<<std::endl;
        std::cout<<"业务执行逻辑"<<std::endl;
        return true;
    }

    //提供rpc服务的远程调用
      virtual void Search(::google::protobuf::RpcController* controller,
                       const ::user_proto::SearchRequest* request,
                       ::user_proto::SearchResponse* response,
                       ::google::protobuf::Closure* done)
    {
                        uint32_t uid = request->uid();
                        //处理业务的本地方法
                        Search(uid,response);

                        //处理完毕，response序列化，和网络发送
                        done->Run();

    }   
    //处理查询业务的本地方法
    bool Search(uint uid,::user_proto::SearchResponse* response){
            std::cout<<"根据uid执行查询"<<std::endl;
            //查询成功返回true,否则false;
            //此处假设返回true;
            user_proto::UserInfo* userInfo = response->mutable_userinfo();
            userInfo->set_uid(uid);
            userInfo->set_name("wang qiang");
            userInfo->set_age(43);
            ::user_proto::ResponseState*state = response->mutable_state();
            state->set_errcode(0);
            state->set_errmsg("");
            response->set_success(true);
        return true;
    }

};


int main(int argc,char** argv){
    //调用框架加载基本信息
    MyrpcApplication::getInstance().init(argc,argv);
    
    MyrpcProvider provider;
    //把服务发布到rpc结点上
    provider.notifyService(new UserService());
    //启动rpc结点，阻塞等待被调用
    provider.run();

}