#include<iostream>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<unistd.h>
using namespace std;
mutex t;
condition_variable cv;
void fun()
{

    unique_lock<mutex>lock(t);
    cv.wait(lock);
    cout<<"sub"<<endl;
}
int main()
{
    thread t(fun);

    
    cout<<"main"<<endl;

    return 0;
}