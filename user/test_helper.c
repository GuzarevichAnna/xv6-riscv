#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{    
    for (int i = 1; i < argc; ++i)
    {
        for (int j = 0; j < strlen(argv[i]); ++j)
        {
            printf("pid %d: arg %d, char '%c\n", getpid(), i, argv[i][j]);
        }
    }

    exit(0);
}