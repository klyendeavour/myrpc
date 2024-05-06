#include "user.pb.h"
#include  "myrpcapplication.h"


int main(int argc,char** argv){
    //框架加载配置
    MyrpcApplication::init(argc,argv);
    //代理桩类，用户调用的远程服务方法都会交给它来完成参数的序列化,进而通过网络库发送给服务提供方
    user_proto::UserServiceRpc_Stub serviceStub(new MyrpcChannel());
    //调用方法的请求参数
    user_proto::LoginRequest request;
    request.set_name("root");
    request.set_pwd("hello54654");
    //响应参数
    user_proto::LoginResponse response;
    //调用rpc服务 Login
    std::cout<<"--------------调用Login服务---------------"<<std::endl;
    MyrpcController loginController;
    serviceStub.Login(&loginController,&request,&response,nullptr);

    if(!loginController.Failed()){//桩类处理时没有出错
        if(response.success()){
        std::cout<<"登录成功"<<std::endl;
        }else{
            const ::user_proto::ResponseState& state = response.state();
            std::cout<<"errcode :" <<state.errcode()<<std::endl;
            std::cout<<"errmsg :" <<state.errmsg()<<std::endl;
        }
    }else{
        std::cout<<loginController.ErrorText()<<std::endl;
    }

    

    //调用rpc服务 Search
     std::cout<<"--------------调用Search服务---------------"<<std::endl;
    user_proto::SearchRequest searchRequest;
    user_proto::SearchResponse searchResponse;
    searchRequest.set_uid(489321);
    MyrpcController searchController;
    serviceStub.Search(&searchController,&searchRequest,&searchResponse,nullptr);

    if(!searchController.Failed()){
         if(response.success()){
        user_proto::UserInfo info = searchResponse.userinfo();
        std::cout<<"name :" <<info.name()<<std::endl;
        std::cout<<"uid :" <<info.uid()<<std::endl;
        std::cout<<"age :" <<info.age()<<std::endl;
        }else{
            user_proto::ResponseState state = searchResponse.state();
            std::cout<<"errcode :" <<state.errcode()<<std::endl;
            std::cout<<"errmsg :" <<state.errmsg()<<std::endl;
        }
    }else{
        std::cout<<searchController.ErrorText()<<std::endl;
    }

   


}

