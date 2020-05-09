#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "head.h"

#define USR_LEN 16
#define STR_LEN 11
#define FNAME_LEN 15

typedef struct{
    int dataLen;//存储的是 buf上到底运了多少数据
    char buf[1000];//存储要发送的数据
}train_t;

//注册信息
typedef struct{
    char uname[USR_LEN];
    char salt[12];
    char cipher[100];
}train_reg;
//网盘路径信息
typedef struct{
    char pathname[5][10];
    int tail;
}pathstr;

int client_login(char *, int);
int read_cmd(int, pathstr*);
int tcp_init_client(int *sfd,char *ip,char *port);

int recvCycle(int newFd,void *pStart,int len);
int trans_cmd(int, void*, int);
int trans_file(int, char*);
int client_register(int);
int trans_reg(int, train_reg);
char *GenerateStr(char* str);

int trans_tag(int socketFd, int cmd_tag);
int cmd2op(char* command);
int cmd_cd(int, char*, pathstr*);
int cmd_ls(int);
int cmd_gets(int, char*);
int cmd_puts(int, char*);
int cmd_pwd(pathstr);
int cmd_remove(int, char*);
int cmd_mkdir(int socketFd, char* fname);
int cmd_exit(int socketFd);

int download(int, char*);
void print_help();
/*
  cmd_tag:
  1 = cd;
  2 = ls;
  3 = puts;
  4 = gets;
  5 = remove;
  6 = pwd;
  7 = login;
  8 = register;
  0 = data;
*/

#endif
