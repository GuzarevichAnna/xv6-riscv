#include "kernel/types.h"
#include "kernel/riscv.h"
#include "kernel/param.h"
#include "kernel/spinlock.h"
#include "kernel/proc.h"
#include "user/user.h"

void print_padded_string(char *str, int width)
{
    int len = strlen(str);
    printf("%s", str);
    for (int i = len; i < width; i++)
    {
        printf(" ");
    }
}

const char* procstate_to_string(enum procstate state) {
    switch(state) {
        case UNUSED: return "UNUSED";
        case USED: return "USED";
        case SLEEPING: return "SLEEPING";
        case RUNNABLE: return "RUNNABLE";
        case RUNNING: return "RUNNING";
        case ZOMBIE: return "ZOMBIE";
        default: return "UNKNOWN";
    }
}

int main(int argc, char *argv[]) {
    int limit = 2;
    struct procinfo *plist = malloc(limit * sizeof(struct procinfo));
    if (plist == 0)
    {
        fprintf(2, "ps: malloc failed\n");
        exit(1);
    }
    while (1)
    {
        int ret = ps_listinfo(plist, limit);
        if (ret == -1)
        {
            limit *= 2;
            free(plist);
            plist = malloc(limit * sizeof(struct procinfo));
            if (plist == 0)
            {
                fprintf(2, "ps: malloc failed\n");
                exit(1);
            }
        }
        else if (ret == -2)
        {
            fprintf(2, "ps: bad user address\n");
            free(plist);
            exit(1);
        }
        else
        {
            for (int i = 0; i < ret; ++i)
            {
                printf("#%d\tpid=%d\tname=", i, plist[i].pid);
                print_padded_string(plist[i].name, 16);
                printf("\tstate=%s\tparent_pid=%d\tparent_name=", procstate_to_string(plist[i].state), plist[i].parent_pid);
                print_padded_string(plist[i].parent_name, 16);
                printf("\n");
            }
            free(plist);
            exit(0);
        }
    }
}