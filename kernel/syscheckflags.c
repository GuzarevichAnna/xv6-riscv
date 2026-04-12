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

uint64 sys_checkflags(void)
{
    uint64 va;
    int len;
    int flags_to_drop;

    argaddr(0, &va);
    argint(1, &len);
    argint(2, &flags_to_drop);

    if (flags_to_drop & ~(PTE_A | PTE_D)) {
        return -1;
    }

    int flag_set = 0;

    while (len > 0)
    {
        uint64 page_va = PGROUNDDOWN(va);

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

        if (flags_to_drop & PTE_A) {
            if (*pte & PTE_A) {
                flag_set = 1;
            }
        }
        if (flags_to_drop & PTE_D) {
            if (*pte & PTE_D) {
                flag_set = 1;
            }
        }

        len -= PGSIZE - (va - page_va);
        va = page_va + PGSIZE;
    }

    return flag_set;
}