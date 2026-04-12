#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int m;

    if (strcmp(argv[1], "\0") == 0) { // case without synchronization
        m = -1;
    } else { // case with synchronization
        m = atoi(argv[1]);
        if (m == 0) {
            fprintf(2, "Error in atoi\n");
        }
    }

    
    for (int i = 2; i < argc; ++i)
    {
        for (int j = 0; j < strlen(argv[i]); ++j)
        {
            if (m >= 0 && mutex_lock(m) < 0) {
                fprintf(2, "Error acquiring mutex\n");
                exit(1);
            }
            printf("pid %d: arg %d, char '%c\n", getpid(), i, argv[i][j]);
            if (m >= 0 && mutex_unlock(m) < 0) {
                fprintf(2, "Error releasing mutex\n");
                exit(1);
            }
        }
    }

    if (m >= 0 && close(m) < 0) {
        fprintf(2, "Error closing mutex\n");
        exit(1);
    }

    exit(0);
}