#include "../include/deskserver.h"
#include "../include/md5.h"

#define TRAINSIZE 200

int recvCmd(int newFd, MYSQL* pmysql, FILE* fp_log)
{
    int fid = sql_max_fid(pmysql);
    int userid = -1;
    int code = 1;
    char uname[16] = {0};
    while(1)
    {
    //train_cmd cmd_t;
    //bzero(&cmd_t, sizeof(cmd_t));
        int cmd_tag = recvCmd_tag(newFd);
    //printf("tag = %d\n", cmd_tag);
        if(-1 == analyse_cmd(cmd_tag,newFd, pmysql, &userid, &code, uname, fp_log, &fid))
        {
            break;
        }
    }
    return 0;
}

//通过recvCmd_tag接收客户端发送的指令码tag，根据收到的tag值，判断客户端的行为，采取相应的措施
int recvCmd_tag(int newFd)
{
    int ret, tag;
    ret = recv(newFd, &tag, sizeof(int), 0);
    if(-1 == ret)
    {
        printf("Receive tag wrong.\n");
        return -1;
    }
    return tag;
}

int analyse_cmd(int cmd_tag,int newFd, MYSQL* pmysql, int* puserid, int *pcode, char* uname, FILE* fp_log, int *pfid)
{
    int ret;
    char affim = 'a';
    switch(cmd_tag)
    {
    case 1:
//printf("receive command cd.\n");
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_cmd_affim");
        ret = get_cd(newFd, *puserid, pcode, pmysql, uname, fp_log);
        break;
    case 2:
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_cmd_affim");
        ret = get_ls(newFd, *puserid, *pcode, pmysql, uname, fp_log);
        break;
    case 3:
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_cmd_affim");
        get_puts(newFd, *puserid, *pcode, pmysql, uname, fp_log, pfid);
        break;
    case 4:
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_cmd_affim");
        get_gets(newFd, *puserid, pmysql, uname, fp_log, *pfid);
        break;
    case 5:
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_cmd_affim");
        get_remove(newFd, *puserid, pmysql, uname, fp_log);
        break;
    case 7:
        *puserid = get_login(newFd, pmysql, uname, fp_log);
        break;
    case 8:
        get_reg(newFd, pmysql, uname, fp_log);
        break;
    case 9:
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_cmd_affim");
        get_mkdir(newFd, pmysql, *puserid, *pcode, uname, fp_log);
        break;
    case 10:
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_cmd_affim");
        write_log(fp_log, uname, "exit", " ");
        write_log(fp_log, uname, "exit", " ");
        return -1;
    }
    return 0;
}

int get_cd(int newFd, int userid, int* pcode, MYSQL* pmysql, char* uname, FILE* fp_log)
{
    char cd_path[FNAME_LEN] = {0};
    int affim_code = 0;
    int ret = recv(newFd, &cd_path, FNAME_LEN, 0);
    ERROR_CHECK(ret, -1, "recv_cd_folder");
 //printf("user%d cd %s/\n", userid, cd_path);
    if(0 != strcmp(cd_path, ".."))
    {
        if(0 ==  sql_query_cd(pmysql, userid, cd_path, pcode))
        {
            affim_code = 3;
        }else{
            //查找错误。
            affim_code = 2; 
        }

    }else{
        if(*pcode == 1)
        {
            //已到达根目录
            affim_code = 2;
        }else{
            //若不是根目录，则必能找到上级目录。
            *pcode =  sql_query_cd_pre(pmysql, *pcode);
            affim_code = 4;
        }
    }
    ret = send(newFd, &affim_code, sizeof(int), 0);
    ERROR_CHECK(ret, -1, "semd_affim_code");
    if((3 == affim_code) || (4 == affim_code))
    {
        write_log(fp_log, uname, "cd", "");
        return 0;
    }else{
        return -1;
    }
}

int get_ls(int newFd, int userid, int code, MYSQL* pmysql, char* uname, FILE* fp_log)
{
    char files[300] = {0};
    sql_query_ls(pmysql, userid, code, files);
    int ret = send(newFd, files, sizeof(files), 0);
    ERROR_CHECK(ret, -1, "send_ls_files");
    write_log(fp_log, uname, "ls", "");
    return 0;
}

int get_gets(int newFd, int userid, MYSQL* pmysql, char* uname, FILE* fp_log, int fid)
{
    char affim = 'a';
    char fname[FNAME_LEN] = {0};
    char md5[33] = {0};
    int ret = recv(newFd, &fname, FNAME_LEN, 0);
    ERROR_CHECK(ret, 0, "recv_file_name");
//printf("filename is %s\n", fname);
    if(0 == sql_gets(pmysql, userid, fname, md5))
    {
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, 0, "send_affim");
        trans_file(newFd, fname, fid);
        ret = send(newFd, &md5, 33, 0);
        ERROR_CHECK(ret, 0, "send_md5");
printf("user \033[;32;1m%s\033[;;1m gets \033[;33;1m%s\033[;;1m\n", uname, fname);
        write_log(fp_log, uname, "gets", fname);
       return 0;
    }else{
        affim = '\0';
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, 0, "send_affim");
        return -1;
    }

}

int get_puts(int newFd, int userid, int code, MYSQL* pmysql, char* uname, FILE* fp_log, int* pfid)
{
    char fname[FNAME_LEN] = {0};
    char fname_id[5] = {0};
    char affim = '\0';
    long int size = 0;
    int ret = recv(newFd, fname, FNAME_LEN, 0 );
    ERROR_CHECK(ret, -1, "recv_fname");
//printf("recv fanme = %s\n ",fname);
    if(0 == strcmp(fname, "!nosuchfile"))
    {
        printf("Client cancel puts.\n");
        return -1;
    }
    if(0 ==  sql_query_fname(pmysql, fname, userid, code)) 
    {
        affim = 'a';
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_fname_affim");
printf("get file \033[;33;1m%s\033[;;1m start, please wait...\n", fname);
        download(newFd, &size, pfid);
        sprintf(fname_id, "%d", *pfid);
        char md5_server[MD5_STR_LEN+1] = {0};
        char md5_client[MD5_STR_LEN+1] = {0};
        ret = recv(newFd, &md5_client, MD5_STR_LEN, 0);
        ERROR_CHECK(ret, -1, "recv_md5");
//printf("client_md5 = %s\n", md5_client);
        ret = Compute_file_md5(fname_id, md5_server);
        ERROR_CHECK(ret, -1, "calc_md5");
//printf("server_md5 = %s\n", md5_server);

        if(0 == strcmp(md5_client, md5_server))
        {//nd5验证成功
            affim = 'a';
            ret = send(newFd, &affim, sizeof(char), 0);
            ERROR_CHECK(ret, -1, "send_md5_affim");
            if(0 == sql_insert_puts(pmysql, userid, fname, code, size, md5_server))
            {
printf("get_puts \033[;33;1m%s\033[;;1m success.\n", fname);
            write_log(fp_log, uname, "puts", fname);
            return 0;
                
            }else{
                goto lable1;
            }
        } else{
    lable1:
            affim = '\0';
            ret = send(newFd, &affim, sizeof(char), 0);
            ERROR_CHECK(ret, -1, "send_md5_affim");
printf("get_puts \033[;33;1m%s\033[;;1m failed.\n", fname);
            return -1;
        }
        return 0;
    }else{
        ret = send(newFd, &affim, sizeof(char), 0);
        ERROR_CHECK(ret, -1, "send_fname_affim");
        return -1;
    }
}

int get_remove(int newFd, int userid, MYSQL* pmysql, char* uname, FILE* fp_log)
{
    char fname[FNAME_LEN] = {0};
    char affim = '\0';
    int ret = recv(newFd, fname, FNAME_LEN, 0 );
    ERROR_CHECK(ret, -1, "recv_fname");
    if(0 == sql_remove(pmysql, fname, userid))
    {
        remove(fname);
        affim = 'a';
        write_log(fp_log, uname, "remove", fname);
        //删除本地文件；
    }
    ret = send(newFd, &affim, sizeof(char), 0);
    ERROR_CHECK(ret, -1, "send_affim");
    return 0;
}

int get_mkdir(int newFd, MYSQL* pmysql, int userid, int code, char* uname, FILE* fp_log)
{
    char fname[FNAME_LEN] = {0};
    char affim = '\0';
    int ret = recv(newFd, fname, FNAME_LEN, 0);
    ERROR_CHECK(ret, -1, "recv_foldername");
printf("sql_query_fname %s\n", fname);
    if(0 == sql_query_fname(pmysql, fname, userid, code))
    {
        affim = 'a';
        send(newFd, &affim ,sizeof(char), 0);
printf("insert mkdir\n");
        sql_insert_mkdir(pmysql, fname, userid, code);
        write_log(fp_log, uname, "mkdir", fname);
        //删除本地文件；
        return 0;
    }else{
        send(newFd, &affim ,sizeof(char), 0);
        return -1;
    }
}

int get_login(int newFd, MYSQL* pmysql, char* uname, FILE* fp_log)
{
    int ret, userid = -1;
    char affim = 'a';
    char salt[12] = {0};
    char cipher[100] = {0};
    ret = send(newFd, &affim, sizeof(affim), 0); 
    ERROR_CHECK(ret, -1, "send_affim");
    ret = recv(newFd, uname, sizeof(uname), 0);
    ERROR_CHECK(ret, -1, "recv_uname");
    userid = sql_query_salt(pmysql, uname, salt);
    ret = send(newFd, salt, sizeof(salt), 0);
    ERROR_CHECK(ret, -1, "send_salt");
    if(0 == strcmp(salt, "nouname"))
    {
        puts("Wrong username.");
        return -1;
    }else{
        ret = recv(newFd, cipher, sizeof(cipher), 0);
        if(0 == sql_cmp_cipher(pmysql, uname, cipher))
        {
printf("User: \033[;32;1m%s\033[;;1m login \033[;31;1msuccessful\033[;;1m.\n", uname);
            ret = send(newFd, &userid, sizeof(int), 0);
            ERROR_CHECK(ret, -1, "send_right_result");
            write_log(fp_log, uname, "login", "");
        }else{
printf("User: \033[;32;1m%s\033[;;1m login \033[;31;1mfailed\033[;;1m.(Wrong password)\n", uname);
            userid = -1;
            ret = send(newFd, &userid, sizeof(int), 0);
            ERROR_CHECK(ret, -1, "send_wrong_result");
        }
    }
    return userid;
}

int get_reg(int newFd, MYSQL* pmysql, char* uname, FILE* fp_log)
{
    int ret;
    char affim = 'a';
    train_reg t_reg;
    ret = send(newFd, &affim, sizeof(affim), 0);
    ERROR_CHECK(ret, -1, "send_affim");
    ret = recv(newFd, &t_reg, sizeof(train_reg), 0);
    ERROR_CHECK(ret, -1, "recv_reg_t");
    if(0 ==  sql_insert_reg(pmysql, t_reg))
    {
        affim = 'r';
        ret = send(newFd, &affim, sizeof(affim), 0);
        ERROR_CHECK(ret, -1, "send_resault");
        printf("User: \033[;32;1m%s\033[;;1m registered successful.\n", t_reg.uname);
        write_log(fp_log, uname, "register", "");
        return 0;
    }else{
        affim = '\0';
        ret = send(newFd, &affim, sizeof(affim), 0);
        return -1;
        printf("Register user: \033[;32;1m%s\033[;;1m failed : username already exist.\n", t_reg.uname);
    }
}

int write_log(FILE* fp_log, char* uname, char* cmd, char* fname)
{
    time_t timep;
    time(&timep);
    char msg[200] = {0};
    char *src = ctime(&timep);
    sprintf(msg, "%s -  user: %s -  %s %s\n", src, uname, cmd, fname);
    msg[strlen(src) - 1] = ' ';
    fseek(fp_log, 0, SEEK_END);
    fwrite(msg, strlen(msg), 1, fp_log);
//printf("%s", msg);
}
