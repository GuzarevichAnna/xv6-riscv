#include "types.h"
#include "param.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "ring_buffer.h"

#define MAX_DMESG_OUTPUT 2048 // this syscall copies only part of ring_buffer to user (up to MAX_DMESG_OUTPUT-1 bytes)

// copy first to tmp_buf in kernel-space, then copyout from tmp_buf to user-space, because copyout can't be used when buf.lock is acquired
uint64 sys_dmesg(void)
{
    uint64 user_dst;
    argaddr(0, &user_dst);

    char tmp_buf[MAX_DMESG_OUTPUT];
    int copied_len = 0;

    acquire(&buf.lock);

    // skip till the first '\n'
    while (buf.head != buf.tail && buf.data[buf.head] != '\n')
    {
        buf.head = (buf.head + 1) % RING_BUFFER_SIZE;
    }
    if (buf.head == buf.tail)
    {
        tmp_buf[0] = '\0';
        goto pass_to_user;
    }
    buf.head = (buf.head + 1) % RING_BUFFER_SIZE; // skip '\n'
    if (buf.head == buf.tail)
    {
        tmp_buf[0] = '\0';
        goto pass_to_user;
    }

    while (buf.head != buf.tail && copied_len < MAX_DMESG_OUTPUT - 1) // -1 is for '\0'
    {
        tmp_buf[copied_len] = buf.data[buf.head];
        ++copied_len;
        buf.head = (buf.head + 1) % RING_BUFFER_SIZE;
    }

    // exclude everything after the last '\n'
    --copied_len;
    while (copied_len >= 0 && tmp_buf[copied_len] != '\n')
    {
        --copied_len;
    }
    if (copied_len < 0) // there are no full messages
    {
        copied_len = 0;
        tmp_buf[0] = '\0';
        goto pass_to_user;
    }
    ++copied_len;
    tmp_buf[copied_len] = '\0';

pass_to_user:
    release(&buf.lock);
    if (copyout(myproc()->pagetable, user_dst, tmp_buf, copied_len + 1) < 0)
    {
        return -1;
    }

    return 0;
}