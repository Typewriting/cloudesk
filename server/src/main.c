#include "../include/deskserver.h"
int main(int argc,char *argv[])
{
    /*if(argc!=5)
    {
        printf("usage:./server IP PORT threadNum queCapacity\n");
    }
*/
    //登陆数据库
    MYSQL mysql;
    sql_login(&mysql);
    FILE* fp_log = fopen("../log/log.txt", "a+");

    Factory_t facData;
    int threadNum=atoi("5");//atoi(argv[3]);
    int queCapacity=atoi("5");//(argv[4]);
    //工厂初始化
    factory_init(&facData,threadNum,queCapacity, &mysql, fp_log);
    //启动线程池
    factory_start(&facData);
    //初始化socket并到listen
    int socketFd;
    tcp_init(&socketFd,"192.168.211.128", "2000");
    chdir ("../storage");
    int newFd;
    pNode_t pNewRequest;
    pQue_t pQueue=&facData.que;//拿到队列的结构体地址
    while(1)
    {
        newFd=accept(socketFd,NULL,NULL);
        pNewRequest=(pNode_t)calloc(1,sizeof(Node_t));
        pNewRequest->newFd=newFd;
        pthread_mutex_lock(&pQueue->queMutex);
        que_set(pQueue,pNewRequest);//放入队列
        pthread_mutex_unlock(&pQueue->queMutex);
        pthread_cond_signal(&facData.cond);//唤醒一个子线程
    }
    mysql_close(&mysql);
    fclose(fp_log);
    return 0;
}
