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

void TestSuccess()
{
    int limit = 2;
    struct procinfo *plist = malloc(limit * sizeof(struct procinfo));
    if (plist == 0)
    {
        fprintf(2, "malloc failed in test code\n");
        printf("TestSuccess failed!\n\n");
        return;
    }
    while (1)
    {
        int ret = ps_listinfo(plist, limit);
        if (ret == -1)
        {
            printf("TestSuccess: limit=%d is too small, increasing it...\n", limit);
            limit *= 2;
            free(plist);
            plist = malloc(limit * sizeof(struct procinfo));
            if (plist == 0)
            {
                fprintf(2, "malloc failed in test code\n");
                printf("TestSuccess failed!\n\n");
                return;
            }
        }
        else if (ret == -2)
        {
            printf("TestSuccess failed! (False bad address detection)\n\n");
            free(plist);
            return;
        }
        else
        {
            printf("TestSuccess: number of processes is %d, their procinfo:\n", ret);
            for (int i = 0; i < ret; ++i)
            {
                printf("#%d\tpid=%d\tname=", i, plist[i].pid);
                print_padded_string(plist[i].name, 16);
                printf("\tstate=%s\tparent_pid=%d\tparent_name=", procstate_to_string(plist[i].state), plist[i].parent_pid);
                print_padded_string(plist[i].parent_name, 16);
                printf("\n");
            }
            printf("TestSuccess passed!\n\n");
            free(plist);
            return;
        }
    }
}

void TestNullBuffer() {
    int limit = 2;
    struct procinfo *plist = (struct procinfo*)0x0;
    int ret = ps_listinfo(plist, limit);
    printf("TestNullBuffer: number of processes is %d\n", ret);
    if (ret > 0) {
        printf ("TestNullBuffer passed!\n\n");
    }
    else {
        printf("TestNullBuffer failed!\n\n");
    }
}

void TestBadAddress() {
    int limit = 2;
    struct procinfo *plist = (struct procinfo*)0x80000000; // kernel address
    int ret = ps_listinfo(plist, limit);
    if (ret == -2) {
        printf("TestBadAddress passed!\n\n");
    } else {
        printf("TestBadAddress failed!\n\n");
    }
}

int main(int argc, char *argv[])
{
    TestSuccess();
    TestNullBuffer();
    TestBadAddress();
    exit(0);
} 
