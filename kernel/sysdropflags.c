#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

uint64 sys_dropflags(void)
{
    uint64 va;
    int len;
    int flags_to_drop;

    argaddr(0, &va);
    argint(1, &len);
    argint(2, &flags_to_drop);

    if (flags_to_drop & ~(PTE_A | PTE_D))
    {
        return -1;
    }

    // check that all pages exist, are valid and are accessible by user
    int len_copy = len;
    uint64 va_copy = va;
    while (len_copy > 0)
    {
        uint64 page_va = PGROUNDDOWN(va_copy);

        pte_t *pte = walk(myproc()->pagetable, page_va, 0);
        if (pte == 0)
        {
            return -2;
        }
        if ((*pte & PTE_V) == 0)
        {
            return -2;
        }
        if ((*pte & PTE_U) == 0)
        {
            return -2;
        }

        len_copy -= PGSIZE - (va_copy - page_va);
        va_copy = page_va + PGSIZE;
    }

    // drop the flags
    while (len > 0)
    {
        uint64 page_va = PGROUNDDOWN(va);

        pte_t *pte = walk(myproc()->pagetable, page_va, 0);

        if (flags_to_drop & PTE_A)
        {
            *pte &= (~PTE_A);
        }
        if (flags_to_drop & PTE_D)
        {
            *pte &= (~PTE_D);
        }

        len -= PGSIZE - (va - page_va);
        va = page_va + PGSIZE;
    }

    return 0;
}