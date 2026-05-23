#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int pid1 = fork();
    if (pid1 < 0)
    {
        fprintf(2, "Fork error\n");
        exit(1);
    }

    if (pid1 == 0)
    {
        // child 1
        char *args[4] = {"test_helper", "first", 0};
        exec("test_helper", args);
        fprintf(2, "Exec error\n");
        exit(1);
    }

    int pid2 = fork();
    if (pid2 < 0)
    {
        fprintf(2, "Fork error\n");
        exit(1);
    }
    if (pid2 == 0)
    {
        // child 2
        char *args[4] = {"test_helper", "second", 0};
        exec("test_helper", args);
        fprintf(2, "Exec error\n");
        exit(1);
    }

    wait(0);
    wait(0);

    exit(0);
}