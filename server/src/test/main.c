#include <func.h>

int main(int argc, char *argv[])
{
    time_t timep;
    time(&timep);
    printf("%s", ctime(&timep));
    printf("%s", ctime(&timep));
    return 0;
}

