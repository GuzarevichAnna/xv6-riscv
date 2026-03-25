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

    printf("Calling kalloc for sleeplock from mutexalloc\n");
    (*f)->sleeplock = (struct sleeplock*)kalloc();
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
    printf("Calling kfree for sleeplock from mutexclose\n");
    kfree((char*)sleeplock);
}