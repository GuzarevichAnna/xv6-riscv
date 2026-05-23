#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"
#include "kernel/param.h"
#include "kernel/spinlock.h"
#include "kernel/ring_buffer.h"

int main(int argc, char *argv[])
{
    char* buf = malloc(RING_BUFFER_SIZE + 1); // + 1 is for '\0'
    if (buf == 0) {
        fprintf(2, "malloc failed in dmesg\n");
        exit(1);
    }
    if (dmesg(buf) < 0)
    {
        fprintf(2, "Error in dmesg\n");
        exit(1);
    }
    printf("%s", buf);
    exit(0);
}
