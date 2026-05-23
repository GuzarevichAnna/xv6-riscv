#include "types.h"
#include "param.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "ring_buffer.h"

// copy first to tmp_buf in kernel-space, then copyout from tmp_buf to user-space, because copyout can't be used when buf.lock is acquired
uint64 sys_dmesg(void)
{
    uint64 user_dst;
    argaddr(0, &user_dst);

    char* tmp_buf = kalloc(); // tmp_buf has size RING_BUFFER_SIZE
    int copied_len = 0;

    acquire(&buf.lock);

    int head = buf.head; // use copy of buf.head so that the original buf.head is not moved

    // skip till the first '\n'
    while (head != buf.tail && buf.data[head] != '\n')
    {
        head = (head + 1) % RING_BUFFER_SIZE;
    }
    if (head == buf.tail)
    {
        tmp_buf[0] = '\0';
        goto pass_to_user;
    }
    head = (head + 1) % RING_BUFFER_SIZE; // skip '\n'
    if (head == buf.tail)
    {
        tmp_buf[0] = '\0';
        goto pass_to_user;
    }

    while (head != buf.tail && copied_len < RING_BUFFER_SIZE - 1) // -1 is for '\0'
    {
        tmp_buf[copied_len] = buf.data[head];
        ++copied_len;
        head = (head + 1) % RING_BUFFER_SIZE;
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