#include "../include/client.h"
//用户登录，若是新用户，则注册
int client_login(char* uname, int socketFd)
{
    puts("===========  Cloudesk ☯ Login  ===========");
    printf("(New users? Enter \"new\".)\nusername: ");
    scanf("%s", uname);
    int userid = -1;
    if(!(0 == strcmp(uname, "new")))
    {
        char* passwd = getpass("passward: ");
        int ret, tag = 7;
        char affim = '0';
        ret = send(socketFd, &tag, sizeof(int), 0);
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
            char salt[12] = {0};
            ret = send(socketFd, uname, sizeof(uname), 0);
            ERROR_CHECK(ret, -1, "send_uname");
            ret = recv(socketFd, salt, sizeof(salt), 0);
            ERROR_CHECK(ret, -1, "recv_salt");
            if(0 == strcmp(salt, "nouname"))
            {
                puts("Wrong username! Please login again.");
                return -1;
            }else{
                char* cipher = crypt(passwd, salt);
                ret = send(socketFd, cipher, 100, 0);
                ERROR_CHECK(ret, -1, "send_cipher");
                ret = recv(socketFd, &userid, sizeof(int), 0);
                ERROR_CHECK(ret, -1, "recv_result");
                if(-1 != userid)
                {
                    printf("Login success(userid = %d). Enjoy cloudesk!(Forget commands? Enter \033[;31;1mhelp\033[;;1m.)\n", userid);
                    return userid;
                }else{
                    puts("Login failed.(Wrong password.)\nForget your password?\nSend message to \033[;34;4mxyzo21@163.com\033[;;1m or visit \033[;34;4mwww.xyzo.top\033[;;1m to find help.");
                    return -1;
                }
            }
        }else{
            puts("Can not revice affim from server.\nPlease check your network connection.");
            return -1;
        }
    }else{
        if(0 == client_register(socketFd))
        {
            printf("Register Success!Please run this program again to login.\n");
        }else{
            printf("Register Error!.Please try again.\n");
        }
        return -1;
    }
}

//新用户注册
int client_register(int socketFd)
{
    char uname[USR_LEN] = {0}; 
    char salt[12] = {0};
    printf("=========== Cloudesk ☯ Register===========\nEnter your username:(No more than 10 character.)\n");
    scanf("%s", uname);
    char* passwd = getpass("Enter your passward:(No more than 10 character.)");
    char* passwd_confirm = getpass("Confirm passward:");
    if(strcmp(passwd, passwd_confirm))
    {
        bzero(passwd_confirm, USR_LEN);
        printf("Please confirm your passward again.\n");
        scanf("%s", passwd_confirm);
        if(0 != strcmp(passwd, passwd_confirm))
        {
            printf("Password do not match.Please regisher again.\n");
            return -1;
        }
    }
    //生成salt值
    strcpy(salt, passwd);
    GenerateStr(salt);
    char* cipher = crypt(passwd, salt);
    train_reg t_reg;
    bzero(&t_reg, sizeof(train_reg));
    strcpy(t_reg.uname, uname);
    strncpy(t_reg.salt, salt, 11);
    strncpy(t_reg.cipher,cipher, sizeof(t_reg.cipher));
    if(0 == trans_reg(socketFd, t_reg))
    {
        return 0;
    }else{
        return -1;
    }
}

int trans_reg(int socketFd, train_reg t_reg)
{
    int ret;
    int tag = 8;
    char affim = '\0';
    ret = send(socketFd, &tag, sizeof(tag), 0);
    ERROR_CHECK(ret, -1, "send_tag");
    ret = recv(socketFd, &affim, sizeof(affim), 0);
    ERROR_CHECK(ret, -1, "recv_affim");
    if('a' == affim)
    {
        ret = send(socketFd, &t_reg, sizeof(train_reg), 0);
        ERROR_CHECK(ret, -1, "send_reg_t");
        ret = recv(socketFd, &affim, sizeof(affim), 0);
        ERROR_CHECK(ret, -1, "recv_resault");
        if('r' == affim)
        {
            return 0;
        }else{
            printf("Username already exist. ");
        }
    }
        return -1;
}
//salt值生成函数
char *GenerateStr(char* str)
{
    int i,flag;
    strcpy(str, "$6$");
    srand(time(NULL));//通过时间函数设置随机数种子，使得每次运行结果随机。
    for(i = 3; i < STR_LEN; i ++)
    {
        flag = rand()%3;
        switch(flag)
        {
        case 0:
            str[i] = rand()%26 + 'a';
            break;
        case 1:
            str[i] = rand()%26 + 'A';
            break;
        case 2:
            str[i] = rand()%10 + '0';
            break;
        }
    }
    printf("salt = %s\n", str);//输出生成的随机数。
    return str;
}
