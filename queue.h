#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue{
public:
    Queue(){

    }

    ~Queue(){
        while(que.empty() == false) que.pop();
    }

    void Abort(bool is){
        abort = is;
        condition.notify_all();
    }

    bool getAbort(){
        return abort;
    }

    bool Push(T val){
        std::unique_lock<std::mutex> lock(mutex_);
        que.push(val);
        lock.unlock();
        condition.notify_one();
        return true;
    }

    bool Pop(T &val, const int time_out){
        std::unique_lock<std::mutex> lock(mutex_);
        if(que.empty()){
            condition.wait_for(lock, std::chrono::milliseconds(time_out),[this]()->bool{
                return false;
            });
        }
        if(que.empty()) return false;
        val = que.front();
        que.pop();
        return true;
    }

    bool Front(T &val){
        std::unique_lock<std::mutex> lock(mutex_);
        if(que.empty()) return false;
        val = que.front();
        return true;
    }

    int Size(){
        std::unique_lock<std::mutex> lock(mutex_);
        return que.size();
    }

private:
    bool abort = false;
    std::condition_variable condition;
    std::mutex mutex_;
    std::queue<T> que;
};

#endif // QUEUE_H
