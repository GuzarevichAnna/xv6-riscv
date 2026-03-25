#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void TestReadWrite()
{
    printf("TestReadWrite started...\n");

    int m = mutex();
    if (m < 0)
    {
        fprintf(2, "TestReadWrite: error creating mutex\n");
        exit(1);
    }

    char buf[1];
    if (read(m, buf, 1) >= 0)
    {
        printf("TestReadWrite failed: read from mutex did not produce error!\n");
    }
    else if (write(m, buf, 1) >= 0)
    {
        printf("TestReadWrite failed: write to mutex did not produce error!\n");
    }
    else
    {
        printf("TestReadWrite passed!\n");
    }

    close(m);
}

void TestLockedMutex()
{
    printf("TestLockedMutex started...\n");

    int m = mutex();
    if (m < 0)
    {
        fprintf(2, "TestLockedMutex: error creating mutex\n");
        exit(1);
    }

    if (mutex_lock(m) < 0)
    {
        fprintf(2, "TestLockedMutex: error acquiring mutex\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0)
    {
        fprintf(2, "TestLockedMutex: error in fork\n");
        exit(1);
    }

    if (pid == 0)
    {
        if (mutex_unlock(m) == 0)
        {
            printf("TestLockedMutex failed: unlocking other process's mutex did not produce an error!\n");
        }
        close(m);
        exit(0);
    }
    else
    {
        wait(0);
        if (mutex_unlock(m) < 0)
        {
            printf("TestLockedMutex failed: unlocking mutex owned by that same process resulted in error!\n");
        }
        else
        {
            printf("TestLockedMutex passed!\n");
        }
        close(m);
    }
}

void TestNotClosedMutex()
{
    printf("TestNotClosedMutex started...\n");
    int m_unlocked = mutex();
    if (m_unlocked < 0)
    {
        fprintf(2, "TestNotClosedMutex: error creating mutex\n");
        exit(1);
    }
    if (mutex_lock(m_unlocked) < 0) {
        fprintf(2, "TestNotClosedMutex: error acquiring mutex\n");
        exit(1);
    }
    if (mutex_unlock(m_unlocked) < 0) {
        fprintf(2, "TestNotClosedMutex: error releasing mutex\n");
        exit(1);
    }
    //not closing

    int m_locked = mutex();
    if (m_locked < 0)
    {
        fprintf(2, "TestNotClosedMutex: error creating mutex\n");
        exit(1);
    }
    if (mutex_lock(m_locked) < 0) {
        fprintf(2, "TestNotClosedMutex: error acquiring mutex\n");
        exit(1);
    }
    // not unlocking, not closing
}

int main()
{
    TestReadWrite();
    printf("\n");
    TestLockedMutex();
    printf("\n");
    TestNotClosedMutex();
    printf("\n");
    exit(0);
}