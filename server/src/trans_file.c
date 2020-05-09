#include "../include/factory.h"
void sigFunc(int signum)
{
    printf("%d is coming\n",signum);
}

int trans_file(int newFd, char* fname, int fid)
{
    signal(SIGPIPE,sigFunc);
    train_t dataTrain;
/*
    //先把文件名传给客户端
    train_t dataTrain;
    dataTrain.dataLen=strlen(fname);//规范是要转换大小端
    strcpy(dataTrain.buf,fname);
    send(newFd,&dataTrain,4+dataTrain.dataLen,0);
*/
    //服务器端的文件以数据库的序号命名
    char fname_id[5] = {0};
    sprintf(fname_id, "%d", fid);
    //发送文件大小给客户端
    struct stat buf;
    stat(fname_id,&buf);
    dataTrain.dataLen=sizeof(buf.st_size);
    memcpy(dataTrain.buf,&buf.st_size,dataTrain.dataLen);
    send(newFd,&dataTrain,4+dataTrain.dataLen,0);
    int size = 0;
    //发送文件内容
    if(buf.st_size < 100*1024*1024)
    {
        int ret = recv(newFd, &size, sizeof(int ), 1);
        ERROR_CHECK(ret, -1, "recv_lseek_size");
        int fd=open(fname_id,O_RDONLY);
        lseek(fd,SEEK_SET, size);
        ERROR_CHECK(fd,-1,"open");
        while((dataTrain.dataLen=read(fd,dataTrain.buf,sizeof(dataTrain.buf))))
        {
            send(newFd,&dataTrain,4+dataTrain.dataLen,0);
        }
        //发结束标志
        dataTrain.dataLen=0;
        send(newFd,&dataTrain,4,0);
    }else{
        int fd=open(fname_id,O_RDWR);
        ERROR_CHECK(fd,-1,"open");
        char *pStart=(char*)mmap(NULL,buf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        ERROR_CHECK(pStart,(char*)-1,"mmap");
        send(newFd,pStart,buf.st_size,0);
    }
        return 0;
}

