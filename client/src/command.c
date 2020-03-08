#include "../include/client.h"
//读取用户输入的指令
int read_cmd(char* uname, int socketFd)
{
    char cmd[5] = {0};
    char path[80] = {0};
    while(1)                //接收命令
    {
        /*  1.cd 进入对应目录
            2.ls 列出相应目录文件
            3.puts 将本地文件上传至服务器
            4.gets 文件名 下载服务器文件到本地
            5.remove 删除服务器上文件
            6.pwd 显示目前所在路径            
            */
        //先接收一个字符串
        scanf("%s", cmd);   
        printf("%s = %d\n", cmd, cmd2op(cmd));
        char cd_folder[10] = {0};
        //将指令转换成操作码,并处理;
        switch(cmd2op(cmd))
        {
        case 1:
            scanf("%s", cd_folder);
            cmd_cd(uname, cd_folder, socketFd);
            break;
        case 2:
            cmd_ls(uname);
            break;
        case 3:
            cmd_puts(uname);
            break;
        case 4:
            cmd_gets(uname);
            break;
        case 5:
            cmd_remove(uname);
            break;
        case 6:
            cmd_pwd(uname);
            break;
        default:
            printf("No such order.\n");

        }
        bzero(cmd, sizeof(cmd));
    }
    return 0;
}

//将指令转换为操作码
int cmd2op(char* command)
{
    if(strcmp(command, "cd") == 0)
        return 1;
    else if(strcmp(command, "ls") == 0)
        return 2;
    else if(strcmp(command, "puts") == 0)
        return 3;
    else if(strcmp(command, "gets") == 0)
        return 4;
    else if(strcmp(command, "remove") == 0)
        return 5;
    else if(strcmp(command, "pwd") == 0)
        return 6;
    else
        return 7;
}

//具体每个指令的执行过程
int cmd_cd(char* uname, char* cd_folder, int socketFd)
{
    /*
       train_cmd t;
       t.cmd_tag = 1;
       strcpy(t.mesg, cd_folder);
       printf("cd to the folder:%s\n",t.mesg);
       printf("%s:cmd_cd success.\n",uname);
       */
    return 0;
}

int cmd_ls(char* uname)
{
    printf("%s:cmd_ls success.\n",uname);
    return 0;

}
int cmd_puts(char* uname)
{
    printf("%s:cmd_puts success.\n",uname);
    return 0;

}
int cmd_gets(char* uname)
{
    //dlonload();
    printf("%s:cmd_gets success.\n",uname);
    return 0;

}
int cmd_remove(char* uname)
{
    printf("%s:cmd_remove success.\n",uname);
    return 0;

}
int cmd_pwd(char* uname)
{
    printf("%s:cmd_pwd success.\n",uname);
    return 0;

}
//待定
/*
int trans_cmd(int socketFd, void* train_t, int tag)
{
    signal(SIGPIPE,sigFunc);
    int ret;
    switch(tag)
    {
    case 7:
    ret = send(socketFd, train_t, sizeof(train_login), 0);
    break;
    case 8:
    ret = send(socketFd, train_t, sizeof(train_reg), 0);
    break;
    }
    if(-1 == ret)
    {
        printf("%d client : trans cmd is falid.\n", socketFd);
        return -1;
    }else{
        puts("send is finished.");
    }
    return 0;
}
*/
