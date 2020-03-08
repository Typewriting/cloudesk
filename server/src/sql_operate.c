//数据库操作
#include "../include/deskserver.h"
int sql_login(MYSQL* pmysql){
    char* host = "192.168.211.128";
    char* username = "root";
    char* passwd = "wu188669";
    char* database = "cloudesk";
    if (NULL == mysql_init(pmysql)){
        printf("mysql_init(): %s\n", mysql_error(pmysql));
        return -1;
    }
    if (NULL == mysql_real_connect(pmysql,
                                   host,
                                   username,
                                   passwd,
                                   database,
                                   0,
                                   NULL,
                                   0)) {
        printf("mysql_real_connect(): %s\n", mysql_error(pmysql));
        return -1;

    }
    printf("1. Connected MySQL successful! \n");
/*
    query_str = "select * from users";
    rc = mysql_real_query(pmysql, query_str, strlen(query_str));
    if (0 != rc) {
        printf("mysql_real_query(): %s\n", mysql_error(pmysql));
        return -1;

    }
    res = mysql_store_result(pmysql);
    if (NULL == res) {
        printf("mysql_restore_result(): %s\n", mysql_error(pmysql));
        return -1;

    }
    */
    return 0;
}

int sql_max_fid(MYSQL* pmysql)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100]="select max(code) from files";
    int maxfid = 0;
    unsigned i;
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
            if((row=mysql_fetch_row(res))!=NULL)
            {
                    maxfid =  atoi(row[0]);
            }
            else{
                printf("SQL:Don't find maxfid\n");
                return -1;
            }
            mysql_free_result(res);
    }
    return maxfid;
}

int sql_query_cd(MYSQL* pmysql, int userid, char* cd_path, int* pcode)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100]="select code from files where size = 0 and";
    sprintf(query,"%s name = \'%s\' and userid = %d and code != %d",query, cd_path, userid, *pcode);
//printf("%s\n", query);
    unsigned i;
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        //if(res)
        //{
            if((row=mysql_fetch_row(res))!=NULL)
            {
                for(i=0;i<mysql_num_fields(res);i++)
                {
//printf("num=%d\n",mysql_num_fields(res));//列数
                    *pcode =  atoi(row[i]);
//printf("code = %d\n", *pcode);
                }
            }
            //     }
            else{
                printf("SQL:Don't find data\n");
                return -1;
            }
            mysql_free_result(res);
    }
//    printf("cd_find success.code = %d\n", *pcode);
    return 0;
}


int sql_query_ls(MYSQL*pmysql, int userid, int code, char* files)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100]="select name,size from files where userid =";
    //找文件 size>0
    sprintf(query,"%s %d and precode = %d and size = 0",query, userid, code);
//   puts(query);
    unsigned i;
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        if(res)
        {
            sprintf(files, "          name |  size(Byte)  ");
            while((row=mysql_fetch_row(res))!=NULL)
            {
// printf("num=%d\n",mysql_num_fields(res));//列数
                for(i=0;i<mysql_num_fields(res);i++)
                {
                    if(0 == i)
                    sprintf(files,"%s\n| \033[;34;1m%12s\033[;;1m", files, row[i]);
                    else
                    sprintf(files,"%s |%12s |", files, row[i]);
                    //printf("%8s ",row[i]);
                }
                //printf("\n");
            }
        }else{
            printf("SQL: Don't find data\n");
            //sprintf(cipher, "nouname");
        }
        mysql_free_result(res);
    }
    //找文件夹，size = 0
    bzero(query, sizeof(query));
    sprintf(query,"select name,size from files where userid = %d and precode = %d and size > 0",userid, code);
//    puts(query);

    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {
//printf("num=%d\n",mysql_num_fields(res));//列数
                for(i=0;i<mysql_num_fields(res);i++)
                {
                    if(0 == i)
                    sprintf(files,"%s\n| %12s", files, row[i]);
                    else
                    sprintf(files,"%s |%12s |", files, row[i]);
                }
                //printf("\n");
            }
        }else{
            printf("SQL: Don't find data\n");
            //sprintf(cipher, "nouname");
        }
        mysql_free_result(res);
    }
    return 0;
}

int sql_query_cd_pre(MYSQL* pmysql, int code)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100]="select precode from files where  size = 0 and code = ";
    sprintf(query,"%s%d",query, code);
    unsigned i;
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        //if(res)
        //{
        if((row=mysql_fetch_row(res))!=NULL)
        {
            for(i=0;i<mysql_num_fields(res);i++)
            {
//printf("num=%d\n",mysql_num_fields(res));//列数
                code =  atoi(row[i]);
//printf("code = %d\n", code);
            }
        }
        //     }
        else{
            printf("SQL:Don't find data\n");
        }
        mysql_free_result(res);
    }
    return code;
}

int sql_gets(MYSQL* pmysql, int userid, char* fname, char* md5)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100]="select md5 from files where userid =";
    sprintf(query,"%s %d and name = \'%s\'",query, userid, fname);
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        //if(res)
        //{
        if((row=mysql_fetch_row(res))!=NULL)
        {
            strcpy(md5, row[0]);
        }
        else{
            printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    if(0 == strlen(md5)) 
        return -1;
    else
        return 0;
}

int sql_query_fname(MYSQL* pmysql, char* fname, int userid, int code)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    int fcode = -1;
    char query[200]="select code from files where name =";
    sprintf(query,"%s \'%s\' and userid = %d and precode = %d",query, fname, userid, code);
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
        return -1;
    }else{
        res=mysql_use_result(pmysql);
        //if(res)
        //{
        if((row=mysql_fetch_row(res))!=NULL)
        {
            fcode = atoi(row[0]);
        }
        else{
            //printf("Don't find data\n");
        }
        mysql_free_result(res);
    }
    if(fcode > 0) 
    {
printf("server already have this file.\n");
        return -1;
    }
    else
    {
//printf("server don't have this file.\n");
        return 0;
    }
}

int sql_insert_puts(MYSQL *pmysql, int userid, char* fname, int code, long int size, char* md5)
{
    char query_str[200] = {0};
    sprintf(query_str, "insert into files(precode, name, userid, size, md5) values(%d, \'%s\', %d, %ld, \'%s\')", code, fname, userid, size, md5);
    //char* query_str = "insert into users(username, salt, cipher) values(username, salt, cipher)";
    int ret = mysql_real_query(pmysql, query_str, strlen(query_str));
    if (0 != ret) {
        printf("mysql_real_query(): %s\n", mysql_error(pmysql));
        return -1;
    }else{
//printf("File \033[;34;1m%s\033[;;1m info inserted SQL success!", fname);
        return 0;
    }
}

int sql_remove(MYSQL* pmysql, char* fname, int userid)
{
    char query[100]="delete from files where name = ";
    sprintf(query,"%s \'%s\' and userid = %d",query, fname, userid);
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
        return -1;
    }else{
        printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(pmysql));
        return 0;
    }
}

int sql_insert_mkdir(MYSQL* pmysql, char* fname, int userid, int code)
{
    char query_str[200] = {0};
    sprintf(query_str, "insert into files(precode, name, userid, size) values(%d, \'%s\', %d, %d)", code, fname, userid, 0);
    int ret = mysql_real_query(pmysql, query_str, strlen(query_str));
    if (0 != ret) {
        printf("mysql_real_query(): %s\n", mysql_error(pmysql));
        return -1;
    }else{
        return 0;
    }
}

int sql_insert_reg(MYSQL* pmysql, train_reg t_reg)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[200]="select id from users where username=";
    sprintf(query,"%s\'%s\'",query, t_reg.uname);
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        //if(res)
        //{
        if((row=mysql_fetch_row(res))!=NULL)
        {
            return -1;
        }
        else{
            goto lable1;
        }
        mysql_free_result(res);
    }
lable1: 
    bzero(query, sizeof(query));
    sprintf(query, "insert into users(username, salt, cipher) values(\'%s\', \'%s\', \'%s\')", t_reg.uname, t_reg.salt, t_reg.cipher);
    //char* query_str = "insert into users(username, salt, cipher) values(username, salt, cipher)";
    int ret = mysql_real_query(pmysql, query, strlen(query));
    if (0 != ret) {
        printf("mysql_real_query(): %s\n", mysql_error(pmysql));
        return -1;
    }else{
//puts("User info inserted SQL success!");
        return 0;
    }
}

int sql_query_salt(MYSQL* pmysql, char* uname, char* salt)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100]="select id,salt from users where username=";
    sprintf(query,"%s\'%s\'",query, uname);
    //puts(query);
    int userid = -1;
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        //if(res)
        //{
        if((row=mysql_fetch_row(res))!=NULL)
        {
            //          printf("num=%d\n",mysql_num_fields(res));//列数
            //for(i=0;i<mysql_num_fields(res);i++)
            userid = atoi(row[0]);
            strcpy(salt, row[1]);
            //printf("\n");
        }
        //     }
        else{
            printf("Don't find data\n");
            sprintf(salt, "nouname");
        }
        mysql_free_result(res);
    }
    return userid;
}

int sql_cmp_cipher(MYSQL* pmysql, char* uname, char* cipher)
{
    MYSQL_RES *res;
    MYSQL_ROW row;
    char cipher_sql[100] = {0};
    char query[100]="select cipher from users where username=";
    sprintf(query,"%s\'%s\'",query, uname);
    //puts(query);
    unsigned i;
    if(mysql_query(pmysql,query))
    {
        printf("Error making query:%s\n",mysql_error(pmysql));
    }else{
        res=mysql_use_result(pmysql);
        if(res)
        {
            while((row=mysql_fetch_row(res))!=NULL)
            {
                //           printf("num=%d\n",mysql_num_fields(res));//列数
                for(i=0;i<mysql_num_fields(res);i++)
                {
                    //printf("%8s ",row[i]);
                    sprintf(cipher_sql, row[i]);
                }
                //printf("\n");
            }
        }else{
            printf("Don't find data\n");
            //sprintf(cipher, "nouname");
        }
        mysql_free_result(res);
    }
    if(0 == strcmp(cipher, cipher_sql))
    {
        return 0;
    }else{
        return -1;
    }
}
