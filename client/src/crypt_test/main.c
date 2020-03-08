#include <func.h>
#include <crypt.h>

int main()
{
    printf("%s\n", crypt("$6$0MRQWrYn", "$6$0MRQWrYn") );
    return 0;
}

