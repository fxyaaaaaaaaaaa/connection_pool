//实现连接池
#ifndef CONNECTIONPOOLS
#define CONNECTIONPOOLS
#include<string>
#include<queue>
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<condition_variable>
#include<functional>
#include"Connection.h"
using namespace std;
class ConnectionPools
{
public:
    // 获取连接池的对象实例
    static ConnectionPools& getConnectionPools();

    // 给外部接口，从连接池获取一个可用空闲连接
    shared_ptr<Connection> getConnection();
   void _stop();
private:
    void produceTask();
    void scanConnection();
    ConnectionPools();
    ~ConnectionPools();
    bool loadConfigFile();
    
    
    string _ip; //mysql的IP地址
    unsigned short _port;   //mysql的端口号 
    string _username;   //mysql的登录用户名
    string _password;   //mysql登录用户密码
    string _dbname;     //连接的数据库名称
    int _initSize;      //连接池的初始化连接量
    int _maxSize;       //连接池的最大连接量
    int _maxFreeTime;   //连接池的最大空闲时间
    int _connectionTimeout;//连接池获取连接的超时时间


    queue<Connection*> _connectionQue; //存储连接的队列
    mutex _queueMutex;  //维护队列线程安全的互斥锁
    atomic_int _connetionCnt ;
    atomic_bool exitt;
    condition_variable cv;//设置条件变量 ，用于连接生成线程和消费线程通信
};

#endif