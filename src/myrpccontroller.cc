#include "myrpccontroller.h"

    
     void MyrpcController::Reset(){
        this->failed = false;
        this->errText = "";
     }
 
     bool MyrpcController::Failed() const {
        return failed;
     }
  
     std::string MyrpcController::ErrorText()const {
        return errText;
     }
 
     void MyrpcController::SetFailed(const std::string& reason){
        this->failed = true;
        this->errText = reason;
     }


    void MyrpcController::StartCancel(){}

     bool MyrpcController::IsCanceled()const {return false;}
     
     void MyrpcController::NotifyOnCancel(google::protobuf::Closure* callback) {}