#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void itoa(int n, char *buffer)
{
    int i = 0;
    while (n > 0)
    {
        buffer[i++] = (n % 10) + '0';
        n /= 10;
    }
    buffer[i] = '\0';

    for (int j = 0; j < i / 2; j++)
    {
        char temp = buffer[j];
        buffer[j] = buffer[i - 1 - j];
        buffer[i - 1 - j] = temp;
    }
}

void Test(int m)
{
    char m_str[8];

    if (m < 0) { // case without synchronization
        m_str[0] = '\0';
    } else { // case with synchronization
        itoa(m, m_str);
    }
    
    int pid1 = fork();
    if (pid1 < 0)
    {
        fprintf(2, "Fork error\n");
        exit(1);
    }

    if (pid1 == 0)
    {
        // child 1
        char *args[4] = {"test_print", m_str, "first", 0};
        exec("test_print", args);
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
        char *args[4] = {"test_print", m_str, "second", 0};
        exec("test_print", args);
        fprintf(2, "Exec error\n");
        exit(1);
    }

    wait(0);
    wait(0);
}

int main(int argc, char *argv[])
{
    Test(-1); // case without synchronization

    printf("\n");

    int m = mutex();
    if (m < 0)
    {
        fprintf(2, "Error creating mutex\n");
        exit(1);
    }
    Test(m); // case with synchronization

    exit(0);
}