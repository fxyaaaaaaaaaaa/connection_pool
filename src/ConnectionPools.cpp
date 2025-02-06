#include "../include/ConnectionPools.h"
#include "../include/Public.h"
ConnectionPools &ConnectionPools::getConnectionPools()
{
    static ConnectionPools instance;
    return instance;
};
bool ConnectionPools::loadConfigFile()
{
    FILE *pf = fopen("../include/mysql.cnf", "r");
    if (pf == nullptr)
    {
        CLOG("mysql.cnf open error!");
        return false;
    }
    while (!feof(pf))
    {
        char line[1024] = "";
        fgets(line, 1024, pf);
        string str = line;
        int idx = str.find('=', 0);
        if (idx == -1)
        {
            continue;
        }

        int endidx = str.find('\n', idx);

        string key = str.substr(0, idx);
        string value = str.substr(idx + 1, endidx - idx - 1);

        if (key == "ip")
        {
            _ip = value;
        }
        else if (key == "port")
        {
            _port = atoi(value.c_str());
        }
        else if (key == "username")
        {
            _username = value;
        }
        else if (key == "password")
        {
            _password = value;
        }
        else if (key == "initSize")
        {
            _initSize = atoi(value.c_str());
        }
        else if (key == "maxSize")
        {
            _maxSize = atoi(value.c_str());
        }
        else if (key == "_maxFreeTime")
        {
            _maxFreeTime = atoi(value.c_str());
        }
        else if (key == "_connectionTimeout")
        {
            _connectionTimeout = atoi(value.c_str());
        }
        else if (key == "dbname")
        {
            _dbname = value;
        }
    }
    return true;
}
ConnectionPools::ConnectionPools()
{
    if (!loadConfigFile())
    {
        CLOG("load error!");
        return;
    }
    exitt=false;
    // 创建初始数量的连接
    for (int i = 0; i < _initSize; i++)
    {
        Connection *p = new Connection;
        if(!p->connection(_ip, _port, _username, _password, _dbname))
        {
            CLOG("conn error!");
        };
        _connectionQue.push(p);
        p->refreshAliveTime();
        _connetionCnt++;
    }
    // 启动一个新线程,作为连接生成者

    thread produce(bind(&ConnectionPools::produceTask, this));
    produce.detach();
    thread scan(bind(&ConnectionPools::scanConnection,this));
    scan.detach();
}
void ConnectionPools::produceTask()
{
    for (;;)
    {
        unique_lock<mutex> lock(_queueMutex);

        while (!_connectionQue.empty())
        {
            cv.wait(lock); //队列不为空，此处生产线程进入等待状态。
            if(exitt)
            {
                return ;
            }
        }

        //连接数量还没有达到上限，继续创建新的连接
        if (_connetionCnt < _maxSize)
        {

            Connection *p = new Connection;
            p->connection(_ip, _port, _username, _password, _dbname);
            _connectionQue.push(p);
            p->refreshAliveTime();
            _connetionCnt++;
        }
        //通知消费者线程，可以进行消费了。
        cv.notify_all();
    }
}
shared_ptr<Connection> ConnectionPools::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    
    if(_connectionQue.empty())
    {
        
        cv.wait_for(lock,chrono::milliseconds(_connectionTimeout));

        if(_connectionQue.empty())
        {
            CLOG("获取连接超时....");
            return nullptr;
        }
    }

    /*
        shared_ptr智能指针析构时，会把connection资源直接delete，先当于调用connection的析构函数，connection就被close了
        需要自己定义删除器，将connection直接归还到queue当中。
    */
   
   shared_ptr<Connection> sp(_connectionQue.front(),[&](Connection*conn)
   {
        //这里是在服务器线程中调用，需要注意线程安全的问题。
        unique_lock<mutex> lock(_queueMutex);
        conn->refreshAliveTime();
        _connectionQue.push(conn);
        _connetionCnt ++ ;
   });
    _connectionQue.pop();
    cv.notify_all();  
    return sp;
}
void ConnectionPools::scanConnection()
{
    for(;;)
    {
        // 通过sleep模拟定时效果
        this_thread::sleep_for(chrono::seconds(_maxFreeTime));

        // 扫描整个队列 ，释放多余的连接
        unique_lock<mutex> lock(_queueMutex);
        while (_connetionCnt > _initSize)
        {
            Connection*p = _connectionQue.front();
            if(p->getAliveeTime() >=_maxFreeTime * 1000)
            {
                _connectionQue.pop();
                _connetionCnt--;
                delete p;
            }
            else
            {
                break; //队头的连接没有超过_maxFree 则其他的元素肯定也没超过
            }
        }

        if(exitt)
        {
            return ;
        }
    }
};
ConnectionPools::~ConnectionPools()
{
    lock_guard<mutex> lock(_queueMutex);
    while (!_connectionQue.empty())
    {
        Connection * p=_connectionQue.front();
        _connectionQue.pop();
         _connetionCnt -- ;
        delete p ;
    }
}; 
void ConnectionPools::_stop()
{
    exitt = true;
    cv.notify_all();
};