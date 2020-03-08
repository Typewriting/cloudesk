#include "../include/client.h"

int main(int argc,char *argv[])
{
    //ARGS_CHECK(argc,3);
    int socketFd, userid = -1;
//建立tcp连接
    tcp_init_client(&socketFd,"192.168.211.128", "2000"); 
// 1.登陆到服务器
    char uname[USR_LEN] = {0};
    userid = client_login(uname, socketFd);
    if(-1 == userid)
    {
        return 0;
    }
    chdir("../storge");
//2.创建一个字符串数组，用来存放路径。
    pathstr path;
    //strcpy(path.pathname[0], "\\");
    bzero(path.pathname[0], 10);
    path.tail = 0;
// 3.读取用户输入的命令
    read_cmd(socketFd, &path);
    return 0;
}
