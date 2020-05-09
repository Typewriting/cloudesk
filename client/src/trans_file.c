#include "../include/client.h"
void sigFunc(int signum)
{
    printf("%d is coming\n",signum);
}

int trans_file(int newFd, char* fname)
{
    signal(SIGPIPE,sigFunc);
    //发送文件名
    train_t dataTrain;
    dataTrain.dataLen=strlen(fname);//规范是要转换大小端
    strcpy(dataTrain.buf,fname);
    send(newFd,&dataTrain,4+dataTrain.dataLen,0);
    printf("Start trans \033[;33;1m%s\033[;;1m please wait...\n", fname);
    //发送文件大小给客户端
    struct stat buf;
    stat(fname,&buf);
    dataTrain.dataLen=sizeof(buf.st_size);
    memcpy(dataTrain.buf,&buf.st_size,dataTrain.dataLen);
    send(newFd,&dataTrain,4+dataTrain.dataLen,0);
    //printf("trans filesize success.\n");
    //发送文件内容
    if(buf.st_size < 100*1024*1024)
    {
        int fd=open(fname,O_RDONLY);
        ERROR_CHECK(fd,-1,"open");
        while((dataTrain.dataLen=read(fd,dataTrain.buf,sizeof(dataTrain.buf))))
        {
            send(newFd,&dataTrain,4+dataTrain.dataLen,0);
        }
        //发结束标志
        dataTrain.dataLen=0;
        int ret = send(newFd,&dataTrain,4,0);
        ERROR_CHECK(ret ,-1,"send_finish");
        return 0;
    }else{
        int fd=open(fname,O_RDWR);
        ERROR_CHECK(fd,-1,"open");
        char *pStart=(char*)mmap(NULL,buf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        ERROR_CHECK(pStart,(char*)-1,"mmap");
        send(newFd,pStart,buf.st_size,0);
    }
//printf("trans file success.\n");
    return 0;
}

