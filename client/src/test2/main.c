#include "../../include/client.h"
#include "../../include/md5.h"


int main(int argc, char *argv[])
{
    chdir("../../storge");
    const char *file_path = "filee";
    struct stat buf;
    stat(file_path,&buf);
    char md5_str[MD5_STR_LEN + 1];
    Compute_file_md5(file_path, md5_str);
    printf("%s\nfile size = %ld\n", md5_str, buf.st_size);
    return 0;
}

