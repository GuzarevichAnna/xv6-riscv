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

char flags[8];

char* get_flags(pte_t* pte) {
    flags[0] = *pte & PTE_R ? 'R' : '_';
    flags[1] = *pte & PTE_W ? 'W' : '_';
    flags[2] = *pte & PTE_X ? 'X' : '_';
    flags[3] = *pte & PTE_U ? 'U' : '_';
    flags[4] = *pte & PTE_G ? 'G' : '_';
    flags[5] = *pte & PTE_A ? 'A' : '_';
    flags[6] = *pte & PTE_D ? 'D' : '_';
    flags[7] = '\0';
    return flags;
}

void print_entries(pagetable_t root, int level) {
    for (int pte_idx = 0; pte_idx < PTECOUNT; ++pte_idx) {
        pte_t *pte = &root[pte_idx];
        if (!(*pte & PTE_V)) {
            continue;
        }

        for (int j = 0; j < level; ++j) {
            printf("\t");
        }
        
        char* flags = get_flags(pte);
        printf("0x%x -> 0x%lx %s\n", pte_idx, PTE2PA(*pte), flags);

        if (level < 2) {
            print_entries((pagetable_t)PTE2PA(*pte), level + 1);
        }
        
    }
}

uint64 sys_listpages(void) {
    pagetable_t pagetable = myproc()->pagetable;
    printf("PAGETABLE 0x%lx\n", (uint64)pagetable);
    print_entries(pagetable, 0);
    return 0;
}