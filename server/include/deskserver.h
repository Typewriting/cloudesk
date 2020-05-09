#ifndef __DESKSERVER_H__
#define __DESKSERVER_H__

#include "factory.h"
#define TRAINSIZE 200
#define FNAME_LEN 15

//客户端注册时传送的结构体：
typedef struct{
    char uname[16];
    char salt[12];
    char cipher[100];
}train_reg;

//接收并分析客户端的指令
int recvCmd_train(int newFd, void * pStart);
int recvCmd_tag(int newFd);
int analyse_cmd(int cmd_tag,int newFd, MYSQL* pmysql, int* puserid, int *pcode, char* uname, FILE* fp_log, int * pfid);
int get_cd(int, int, int*, MYSQL*, char* uanme, FILE* fp_log);
int get_ls(int socketFd, int userid, int code, MYSQL* pmysql, char* uanme, FILE* fp_log);
int get_gets(int newFd, int userid, MYSQL* pmysql, char* uname, FILE* fp_log, int fid);
int get_puts(int newFd, int userid, int code, MYSQL* pmysql, char* uname, FILE* fp_log, int* pfid);
int get_login(int newFd, MYSQL* pmysql, char* uanme, FILE* fp_log);
int get_reg(int newFd, MYSQL* pmysql, char* uanme, FILE* fp_log);
int get_remove(int newFd, int userid, MYSQL* pmysql, char* uanme, FILE* fp_log);
int sql_insert_mkdir(MYSQL* pmysql, char* fname, int userid, int code);
int get_mkdir(int newFd, MYSQL* pmysql, int userid, int code, char* uname, FILE* fp_log, int* pfid);
int trans_file(int, char*, int);
int recvCycle(int newFd,void *pStart,int len);
int download(int socketFd, long int *size, int *pfid);

//数据库操作
int sql_max_fid(MYSQL* pmysql);
int sql_query_cd(MYSQL* pmysql, int userid, char* cd_path, int* pcode);
int sql_query_cd_pre(MYSQL* pmysql, int pcode);
int sql_query_ls(MYSQL*pmysql, int userid, int code, char*);
int sql_gets(MYSQL*pmysql, int userid, char*,char*);
int sql_login(MYSQL*);
int sql_insert_puts(MYSQL *pmysql, int userid, char* fname, int code, long int size, char* md5);
int sql_remove(MYSQL* pmysql, char* fname, int userid);
int sql_insert_reg(MYSQL* pmysql, train_reg t_reg);
int sql_query_salt(MYSQL* pmysql, char* uname, char* salt);
int sql_cmp_cipher(MYSQL* pmysql, char* uname, char* cipher);
int sql_query_fname(MYSQL* pmysql, char* fname, int userid, int code);

int write_log(FILE* fp_log, char* uname, char* cmd, char* fname);
#endif
