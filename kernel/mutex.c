#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

int
mutexalloc(struct file **f)
{
    *f = 0;
    *f = filealloc();
    if (*f == 0) {
        return -1;
    }

    (*f)->sleeplock = (struct sleeplock*)kalloc();
    printf("mutexalloc: executed kalloc for sleeplock (mem address = 0x%lx)\n", (uint64)((*f)->sleeplock));
    if ((*f)->sleeplock == 0) {
        fileclose(*f);
        return -1;
    }

    initsleeplock((*f)->sleeplock, "mutex");

    (*f)->type = FD_MUTEX;
    (*f)->readable = 0;
    (*f)->writable = 0;

    return 0;
}

void
mutexclose(struct sleeplock *sleeplock) {
    printf("mutexclose: calling kfree for sleeplock (mem address = 0x%lx)\n", (uint64)sleeplock);
    kfree((char*)sleeplock);
}