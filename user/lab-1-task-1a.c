#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int pid = fork();
    if (pid < 0)
    {
        printf("Fork error\n");
        exit(1);
    }
    else if (pid > 0)
    {
        // in parent
        printf("Parent pid = %d, child pid = %d\n", getpid(), pid);
        int child_exit_status;
        pid = wait(&child_exit_status);
        if (pid == -1)
        {
            printf("Error: parent has no children?!\n");
            exit(1);
        }
        printf("Exited child pid = %d, child exit status = %d\n", pid, child_exit_status);
        exit(0);
    }
    else
    {
        // in child
        uint64 pause_ret_code = pause(100);
        if (pause_ret_code == -1)
        {
            printf("Error in pause in child\n");
        }
        exit(1);
    }
}