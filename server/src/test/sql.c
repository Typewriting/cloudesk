#include <func.h>

int main(int argc, char *argv[])
{
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
