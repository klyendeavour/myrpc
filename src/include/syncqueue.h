#pragma once
#include "mutex"
#include "condition_variable"
#include "queue"

template<typename T>
class SyncQueue{

public:
    //把任务放入队列
    void syncPush(T&task){
        std::lock_guard<std::mutex> lock(m_lock);
        m_queue.push(task);
        m_cond.notify_all();
    }
    //把任务从队列取出
    T syncPop(){
        std::unique_lock<std::mutex> lock(m_lock);
        while(m_queue.empty()){
            m_cond.wait(lock);
        }
        T t =  m_queue.front();
        m_queue.pop();
        return t;
    }
    int syncSize(){
        std::lock_guard<std::mutex> lock(m_lock);
        return m_queue.size();
    }
private:
    std::mutex m_lock;
    //存放任务
    std::queue<T> m_queue;
    std::condition_variable m_cond;

};