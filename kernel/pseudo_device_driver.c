#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"
#include "fcntl.h"

#define BUF_SIZE 128

// buffer to get zeroes from
static char zero_buf[BUF_SIZE] = {0};

struct urandom
{
    struct spinlock lock;

    // new_seed = (a * seed + c) % m
    char seed;
    char a;
    char c;
    short m;
} urandom;

struct nullstat
{
    struct spinlock lock;

    uint64 counter;
} nullstat;

//
// user read()s from pseudo-devices go here.
// user_dst indicates whether dst is a user
// or kernel address.
//
int pseudoread(short minor, int user_dst, uint64 dst, int n)
{
    switch (minor)
    {
    case minor(NULL):
    {
        return 0;
    }
    case minor(ZERO):
    {
        int bytes_written_count = 0;
        while (bytes_written_count < n)
        {
            int bytes_writing_now_count = n - bytes_written_count >= BUF_SIZE ? BUF_SIZE : n - bytes_written_count;
            if (either_copyout(user_dst, dst + bytes_written_count, zero_buf, bytes_writing_now_count) == -1)
            {
                break;
            }
            bytes_written_count += bytes_writing_now_count;
        }
        return bytes_written_count;
    }
    case minor(URANDOM):
    {
        acquire(&urandom.lock);
        urandom.seed = (urandom.a * urandom.seed + urandom.c) % urandom.m;
        if (either_copyout(user_dst, dst, &urandom.seed, sizeof(urandom.seed)) == -1)
        {
            release(&urandom.lock);
            return -1;
        }
        release(&urandom.lock);
        return 1;
    }
    case minor(NULLSTAT):
    {
        acquire(&nullstat.lock);
        if (n != sizeof(nullstat.counter))
        {
            release(&nullstat.lock);
            return -1;
        }
        if (either_copyout(user_dst, dst, &nullstat.counter, sizeof(nullstat.counter)) == -1)
        {
            release(&nullstat.lock);
            return -1;
        }
        release(&nullstat.lock);
        return sizeof(uint64);
    }
    default:
    {
        printf("Unknown pseudo-device\n");
        return -1;
    }
    }
}

//
// user write() system calls to the pseudo-devices go here.
//
int pseudowrite(short minor, int user_src, uint64 src, int n)
{
    switch (minor)
    {
    case minor(NULL):
    {
        return n;
    }
    case minor(ZERO):
    {
        return -1;
    }
    case minor(URANDOM):
    {
        acquire(&urandom.lock);
        if (n != sizeof(urandom.seed))
        {
            release(&urandom.lock);
            return -1;
        }
        if (either_copyin(&urandom.seed, user_src, src, n) == -1)
        {
            release(&urandom.lock);
            return -1;
        }
        release(&urandom.lock);
        return n;
    }
    case minor(NULLSTAT):
    {
        acquire(&nullstat.lock);
        nullstat.counter += n;
        release(&nullstat.lock);
        return n;
    }
    default:
    {
        printf("Unknown pseudo-device\n");
        return -1;
    }
    }
}

void pseudoinit(void)
{
    initlock(&urandom.lock, "urandom");
    urandom.seed = 1;
    urandom.a = 9;
    urandom.c = 13;
    urandom.m = 256;

    initlock(&nullstat.lock, "nullstat");
    nullstat.counter = 0;

    // connect read and write system calls
    // to nullread and nullwrite.
    devsw[major(NULL)].read = pseudoread;
    devsw[major(NULL)].write = pseudowrite;
}
