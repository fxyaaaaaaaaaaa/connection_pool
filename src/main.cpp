#include"../include/Connection.h"
#include"../include/ConnectionPools.h"
int main()
{
   
    //  Connection p1;
    //  p1.connection("127.0.0.1",3306,"root","123456","chat");
    clock_t begin=clock();
    // for(int i=0; i < 100;i++)
    // {
    //     Connection conn;
    //     char sql[1024]="";
    //     sprintf(sql,"insert into users(name) values('%s')","123");
    //     conn.connection("127.0.0.1",3306,"root","123456","chat");
    //     conn.update(sql);
    // }
    ConnectionPools &p= ConnectionPools::getConnectionPools();
    for(int i=0;i < 100;i++)
    {
        char sql[1024]="";
        sprintf(sql,"insert into users(name) values('%s')","123");
        p.getConnection()->update(sql);
    }
    clock_t end=clock();
    cout<<end - begin<<endl;
    p._stop();
    return 0;
}