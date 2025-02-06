//实现MySQL数据库的增删改查操作
#ifndef CONNECTION
#define CONNECTION
#include "mysql/mysql.h"
#include "Public.h"
#include<string>
#include<ctime>
using namespace std;
class Connection
{
public:
    Connection();
    ~Connection();   
    bool connection(string ip,unsigned short port,string user,string password,string dbname);
    bool update(string sql);
    MYSQL_RES* query(string sql);

    // 刷新一下连接的起始空闲时间点
    void refreshAliveTime(){_alivetime = clock();}

    // 返回存活的时间
    clock_t getAliveeTime()const{return clock()-_alivetime;}
private:
    MYSQL* _conn;
    clock_t _alivetime;
};

#endif //Connection