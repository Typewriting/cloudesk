//trains_file直接复制过来的，还没改
//下载文件
#include "../include/deskserver.h"

int recvCycle(int socketFd, void *pStart, int len)
{
    char *p=(char*)pStart;
    int total=0,ret;
    while(total<len)
    {
        ret=recv(socketFd, p+total, len-total, 0);
        if(0==ret)
        {
            printf("byebye\n");
            return -1;
        }
        total=total+ret;
    }
    return 0;
}

int download(int socketFd, long int *size, int *pfid)
{
    int dataLen;
    char buf[1000]={0};
    //先接文件名
    recvCycle(socketFd,&dataLen,4);//拿火车头
    recvCycle(socketFd,buf,dataLen);//拿车厢
    int fd, ret;
    int i=1;
    //char tmp[1000];
    (*pfid)++;
    char fname[10] = {0};
    sprintf(fname, "%d", *pfid);
    //strcpy(tmp,buf);
    //接收文件大小
    off_t fileSize,downLoadSize=0,fileSlice,lastLoadSize=0;
    recvCycle(socketFd,&dataLen,4);
    recvCycle(socketFd,&fileSize,dataLen);
    fileSlice=fileSize/10000;
    *size = fileSize;
    if(*size < 100*1024*1024)
    {
        while((fd=open(fname,O_WRONLY|O_CREAT|O_EXCL,0666))==-1)
        {
            bzero(fname, sizeof(fname));
            sprintf(fname, "%d", ++(*pfid));
            i++;
        }
        while(1)
        {
            ret=recvCycle(socketFd,&dataLen,4);//拿火车头
            if(0==ret&&dataLen>0)
            {
                ret=recvCycle(socketFd,buf,dataLen);//拿车厢
                if(-1==ret)
                {
                    break;
                }
                write(fd,buf,dataLen);//把接到的数据写到磁盘上
                downLoadSize+=dataLen;
                if(downLoadSize-lastLoadSize>=fileSlice)
                {
                    printf("%5.2f%s\r",(double)downLoadSize/fileSize*100,"%");
                    fflush(stdout);
                    lastLoadSize=downLoadSize;
                }
            }else{
                printf("100.00%s\n","%");
                break;
            }
        }
    }else{
        while((fd=open(fname,O_RDWR|O_CREAT|O_EXCL,0666))==-1)
        {
            bzero(fname, sizeof(fname));
            sprintf(fname, "%d", ++(*pfid));
            i++;
        }
//printf("mmap start.\n");
        //接文件内容
        struct timeval start,end;
        gettimeofday(&start,NULL);
        ftruncate(fd,fileSize);//做出对应大小的文件
        char *pStart=(char*)mmap(NULL,fileSize,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
        ERROR_CHECK(pStart,(char*)-1,"mmap");
        ret=recvCycle(socketFd,pStart,fileSize);//拿数据
        if(0==ret)
        {
            munmap(pStart,fileSize);
            gettimeofday(&end,NULL);
            printf("100.00%s\n","%");
            printf("use time=%ld\n",(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec);
        }else{
            printf("error load\n");
        }
    }
    return 0;
}
