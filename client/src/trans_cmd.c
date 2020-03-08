#include "../include/client.h"
#include "../include/md5.h"
//读取用户输入的指令
int read_cmd(int socketFd, pathstr* ppath)
{
    char cmd[7] = {0};
    char cd_name[FNAME_LEN] = {0};
    while(1)                //接收命令
    {
        /*  1.cd 进入对应目录
            2.ls 列出相应目录文件
            3.puts 将本地文件上传至服务器
            4.gets 文件名 下载服务器文件到本地
            5.remove 删除服务器上文件
            6.pwd 显示目前所在路径            
            9,mkdir 创建文件夹
            */
        //接收指令
        scanf("%s", cmd);   
//printf("%s = %d\n", cmd, cmd2op(cmd));
        //将指令转换成操作码,并处理;
        switch(cmd2op(cmd))
        {
        case 1://cd
            cmd_cd(socketFd, cd_name, ppath);
            break;
        case 2://ls
            cmd_ls(socketFd);
            break;
        case 3://puts
            cmd_puts(socketFd, cd_name);
            break;
        case 4://gets
            cmd_gets(socketFd, cd_name);
            break;
        case 5://remove
            cmd_remove(socketFd, cd_name);
            break;
        case 6://pwd
           cmd_pwd(*ppath);
            break;
        case 9://mkdir
            cmd_mkdir(socketFd, cd_name);
            break;
        case 10://exit
            cmd_exit(socketFd);
            return 0;
        case 11://help
            print_help();
            break;
        default://other
            printf("Input wrong command. (Forget commands? Enter \"help\".)\n");
        }
        bzero(cmd, sizeof(cmd));
        bzero(cd_name, sizeof(cd_name));
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
    //7 = login
    //8 = register
    else if(strcmp(command, "mkdir") == 0)
        return 9;
    else if(strcmp(command, "exit") == 0)
        return 10;
    else if(strcmp(command, "help") == 0)
        return 11;
    else
        return -1;
}

//具体每个指令的执行过程
int cmd_cd(int socketFd, char* cd_name, pathstr* ppath)
{
    if(trans_tag(socketFd, 1))
    {
        return -1;
    }
    scanf("%s", cd_name);
    if(0 == strcmp(cd_name, "..") &&( ppath->tail > 0))
    {
        ppath->tail--;
    }else{
        ppath->tail++;
        strcpy(ppath->pathname[ppath->tail], cd_name);
    }
    int affim_code = 0;
//printf("cd_folder = %s\n", cd_path);
    int ret = send(socketFd, cd_name, FNAME_LEN, 0);
    ERROR_CHECK(ret, -1, "send_cd_folder");
    ret = recv(socketFd, &affim_code, sizeof(int), 0);
    ERROR_CHECK(ret, -1, "recv_affim");
    if(4 == affim_code)
    {
//printf("cd up success.\n");
        return 0;
    }else if(3 == affim_code)
    {
//printf("cd success.\n");
        return 0;
    }else if(1 == affim_code)
    {
        printf("Is already the root directory.\n");                         
    }else if(2 == affim_code)
    {
        printf("Input the wrong folder name.\n");
    }else if(0 == affim_code)
    {
        printf("Can't receive affim_code.\n");
    }
        return -1;
}

int cmd_ls(int socketFd)
{
    if(trans_tag(socketFd, 2))
    {
        return -1;
    }else{
    char files[300] = {0};
    int ret = recv(socketFd, &files, sizeof(files), 0);
    ERROR_CHECK(ret, -1, "recv_ls");
    printf("%s\n", files);
    return 0;
    }
}
        
int cmd_gets(int socketFd, char* fname)
{
    if(0 == trans_tag(socketFd, 4))
    {
        scanf("%s", fname);//这里借用cd_name，存放gets的文件名，节省空间
    }else{
        return -1;
    }
    char affim = '\0';
    char md5str[MD5_STR_LEN+1] = {0};
    char md5str_server[MD5_STR_LEN+1] = {0};
    int ret = send(socketFd, fname, FNAME_LEN, 0);
    ERROR_CHECK(ret, -1, "send_fname");
    ret = recv(socketFd, &affim, sizeof(char), 0);
    ERROR_CHECK(ret, -1, "recv_affim");
    if('a' == affim)
    {
        download(socketFd);
        ret = recv(socketFd, &md5str_server, MD5_STR_LEN+1, 0);
        ERROR_CHECK(ret, -1, "recv_md5");
        ret = Compute_file_md5(fname, md5str);
        if(0 == strcmp(md5str, md5str_server))
        {
             puts("Md5 verification successful.");
            return 0;
        }else{
            return -1;
        }
    }
    else{
        printf("Can't find the file. Please enter right file name.\n");
        return -1;
    }
}

int cmd_puts(int socketFd, char* fname)
{
    if(0 == trans_tag(socketFd, 3))
    {
//先检查本地有无此文件
        scanf("%s", fname);
        struct stat buf;
        bzero(&buf, sizeof(buf));
        stat(fname,&buf);
//printf("fsize = %ld\n", buf.st_size);
        if(0 == buf.st_size)
        {
            strcpy(fname,"!nosuchfile");
            send(socketFd, fname, FNAME_LEN,0);
            printf("No such file name.\n");
            return -1;
        }else{
//再检查服务器端有无重复的名字
            char affim = '\0';
            int ret = send(socketFd, fname, FNAME_LEN,0);
            ERROR_CHECK(ret, -1, "send_fname");
            ret = recv(socketFd, &affim, sizeof(char), 0);
            ERROR_CHECK(ret, -1, "recv_affim");
            if('a' == affim)
            {
                trans_file(socketFd, fname);
                char md5str[MD5_STR_LEN+1] = {0};
                ret = Compute_file_md5(fname, md5str);
                ERROR_CHECK(ret, -1, "md5_calc");
printf("client md5 = %s.\n", md5str);
                ret = send(socketFd, &md5str, MD5_STR_LEN+1, 0);
                ERROR_CHECK(ret, -1, "send_md5");
                affim = '\0';
                ret = recv(socketFd, &affim, sizeof(char), 0);
                ERROR_CHECK(ret, -1, "recv_affim");
                if('a' == affim)
                {
                    printf("Puts \033[;33;1m%s\033[;;1m success.\n", fname);
                    return 0;
                }else{
                    printf("Puts \033[;33;1m%s\033[;;1m failed.\n", fname);
                    return -1;
                }
            }else{
                printf("Server already have this name.\n");
                return -1;
            }
        }
    }else{
        return -1;
    }
}

int cmd_pwd(pathstr path)
{
    for(int i = 0; i <= path.tail; i++) 
    {
        printf("\033[;34;1m%s/\033[;;1m", path.pathname[i]);
    }
    printf("\n");
    return 0;
}

int cmd_remove(int socketFd, char* fname)
{
    if(0 == trans_tag(socketFd, 5))
    {
        scanf("%s", fname);//这里借用cd_name，存放gets的文件名，节省空间
    }else{
        return -1;
    }
    char affim = '\0';
    int ret = send(socketFd, fname, FNAME_LEN, 0);
    ERROR_CHECK(ret, -1, "send_fname");
    ret = recv(socketFd, &affim, sizeof(char), 0);
    ERROR_CHECK(ret, -1, "recv_affim");
    if('a' == affim)
    {
         printf("Remove success.\n");
         return 0;

    }else{
        printf("Please enter the true file name.\n");
        return -1;
    }
}

int cmd_mkdir(int socketFd, char* fname)
{
    if(0 == trans_tag(socketFd, 9))
    {
        scanf("%s", fname);//这里借用cd_name，存放gets的文件名，节省空间
    }else{
        return -1;
    }
    char affim = '\0';
    printf("send fname %s\n", fname);
    int ret = send(socketFd, fname, FNAME_LEN, 0);
    ERROR_CHECK(ret, -1, "send_fname");
    ret = recv(socketFd, &affim, sizeof(char), 0);
    ERROR_CHECK(ret, -1, "recv_affim");
    if('a' == affim)
    {
        return 0;
    }else{
        printf("Error: Folder name \033[;34;1m%s\033[;;1m already exist.\n", fname);
        return -1;
    }
}

int cmd_exit(int socketFd)
{
    int ret = trans_tag(socketFd, 10);
    ERROR_CHECK(ret, -1, "exit");
    close(socketFd);
    printf("Bye!\n");
}

int trans_tag(int socketFd, int cmd_tag)
{
    char affim = '0';
    int ret = send(socketFd, &cmd_tag, sizeof(int), 0);
    //ERROR_CHECK(ret, -1, "send_login_tag");
    if(-1 == ret)
    {
        printf("Can't connect server.Please check running statement of server.\n");
        return -1;
    }
    ret = recv(socketFd, &affim, sizeof(char), 0);
    ERROR_CHECK(ret, -1, "recv_affim");
    if('a' == affim)
    {
        //printf("affim successful.\n");
        return 0;
    }else{
        printf("affim failed.\n");
        return -1;
    }
}

void print_help(){
    puts("\033[;31;1mcd \"test\"\033[;;1m     --  open folder named \"test\"");
    puts("\033[;31;1mls\033[;;1m            --  show all files in current folder");
    puts("\033[;31;1mputs \"test\"\033[;;1m   --  upload a file named \"test\" to cloudesk");
    puts("\033[;31;1mgets \"test\"\033[;;1m   --  downloadload a file named \"test\"");
    puts("\033[;31;1mremove \"test\"\033[;;1m --  delete a file named \"test\" from  cloudesk");
    puts("\033[;31;1mpwd\033[;;1m           --  show current path");
    puts("\033[;31;1mexit\033[;;1m          --  you know that ");
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
