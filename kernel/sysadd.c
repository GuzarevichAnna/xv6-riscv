#include "types.h"
#include "riscv.h"
#include "defs.h"

uint64 sys_add(void)
{
    int32 a;
    int32 b;
    argint(0, &a);
    argint(1, &b);
 
    if (a < 0 || b < 0) {
        return -1; // this syscall performs only unsigned addition
    }
    
    uint64 sum = (uint64)a + b;
    return sum;
}
