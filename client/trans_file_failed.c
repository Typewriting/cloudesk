void sigFunc(int signum)
{
    printf("%d is coming\n",signum);
}
int trans_file(int socketFd, char *fname)
{
    signal(SIGPIPE,sigFunc);
    char affim = '\0';
    //发送文件名
    train_t dataTrain;
    struct stat buf;
    stat(fname,&buf);
    dataTrain.dataLen=strlen(fname);//规范是要转换大小端
    //若本地没有这个文件，则buf.st_size为0
    if(0 == buf.st_size)
    {
        strcpy(dataTrain.buf,"!nosuchfile");
        send(socketFd,&dataTrain,4+dataTrain.dataLen,0);
        return -1;
    }else{

    }
    int ret = recv(socketFd, &affim, sizeof(char), 0);
    ERROR_CHECK(ret, -1, "recv_fname_affim");
    if('0' == affim)
    {
        printf("Server already have file named \033[;34;1m%s\033[;;1m.\n", fname);
        return -1;
    }
    strcpy(dataTrain.buf,fname);
    send(socketFd,&dataTrain,4+dataTrain.dataLen,0);
    //发送文件大小给客户端
    dataTrain.dataLen=sizeof(buf.st_size);
    memcpy(dataTrain.buf,&buf.st_size,dataTrain.dataLen);
    send(socketFd,&dataTrain,4+dataTrain.dataLen,0);
    //发送文件内容
    int fd=open(fname,O_RDONLY);
    ERROR_CHECK(fd,-1,"open");
    while((dataTrain.dataLen=read(fd,dataTrain.buf,sizeof(dataTrain.buf))))
    {
        send(socketFd,&dataTrain,4+dataTrain.dataLen,0);
    }
    //发结束标志
    dataTrain.dataLen=0;
    send(socketFd,&dataTrain,4,0);
    return 0;
}


