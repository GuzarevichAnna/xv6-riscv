#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define alloc_size 10 * PGSIZE

int global_var;
char *heap_buf;

void check_flags(uint64 addr, int len, int expected_flags, int allow_bad_address, int allow_any_flags)
{
    printf("Checking flags A and D: ");
    int flag_set = checkflags(addr, len, PTE_A | PTE_D);
    switch (flag_set)
    {
    case -1:
        fprintf(2, "Error in checkflags: bad flags\n");
        exit(1);
    case -2:
        if (allow_bad_address)
        {
            fprintf(2, "Bad address in checkflags\n");
            return;
        }
        else
        {
            fprintf(2, "Error in checkflags: wrong address\n");
            exit(1);
        }
    default:
        printf("%d\n", flag_set);
        if(expected_flags != flag_set && !allow_any_flags) {
            printf("Flags are incorrect\n");
            exit (1);
        };
    }
}

void TestVariableStack() {
    printf("\n\nTestVariableStack started...\n");

    volatile int var;

    printf("Initial state:\n");
    listpages();
    check_flags((uint64)&var, sizeof(var), 0, 0, 1);    // flags on this page may be already set if some other variables on it were accessed/written

    var = 1;
    printf("\n\nAfter writing to stack variable:\n");
    listpages();
    check_flags((uint64)&var, sizeof(var), 1, 0, 0);

    switch (dropflags((uint64)&var, sizeof(var), PTE_A | PTE_D))
    {
    case -1:
        fprintf(2, "Error in dropflags: wrong flags\n");
        exit(1);
    case -2:
        fprintf(2, "Error in dropflags: bad address\n");
        exit(1);
    }
    printf("\n\nAfter dropping A and D:\n");
    listpages();
    check_flags((uint64)&var, sizeof(var), 0, 0, 1);    // flags on this page may still be set if some other variables on it were accessed/written

    [[maybe_unused]] volatile int var_copy = var;
    printf("\n\nAfter reading:\n");
    listpages();
    check_flags((uint64)&var, sizeof(var), 1, 0, 0);

    printf("TestVariableStack succeeded\n");
}

void TestVariableGlobal() {
    printf("\n\nTestVariableGlobal started...\n");

    printf("Initial state:\n");
    listpages();
    check_flags((uint64)&global_var, sizeof(global_var), 0, 0, 0);

    global_var = 1;
    printf("\n\nAfter writing to global variable:\n");
    listpages();
    check_flags((uint64)&global_var, sizeof(global_var), 1, 0, 0);

    switch (dropflags((uint64)&global_var, sizeof(global_var), PTE_A | PTE_D))
    {
    case -1:
        fprintf(2, "Error in dropflags: wrong flags\n");
        exit(1);
    case -2:
        fprintf(2, "Error in dropflags: bad address\n");
        exit(1);
    }
    printf("\n\nAfter dropping A and D:\n");
    listpages();
    check_flags((uint64)&global_var, sizeof(global_var), 0, 0, 0);
    [[maybe_unused]] volatile int var_copy = global_var;
    printf("\n\nAfter reading:\n");
    listpages();
    check_flags((uint64)&global_var, sizeof(global_var), 1, 0, 0);

    printf("TestVariableGlobal succeeded\n");
}

void TestArrayStack()
{
    printf("\n\nTestArrayStack started...\n");

    int stack_buf_size = 512;
    char stack_buf [stack_buf_size];

    printf("Initial state:\n");
    listpages();
    check_flags((uint64)stack_buf, stack_buf_size, 0, 0, 1);    // flags on this page may be already set if some other variables on it were accessed/written

    memset(stack_buf, 1, stack_buf_size);
    printf("\n\nAfter writing to stack memory:\n");
    listpages();
    check_flags((uint64)stack_buf, stack_buf_size, 1, 0, 0);

    switch (dropflags((uint64)stack_buf, stack_buf_size, PTE_A | PTE_D))
    {
    case -1:
        fprintf(2, "Error in dropflags: wrong flags\n");
        exit(1);
    case -2:
        fprintf(2, "Error in dropflags: bad address\n");
        exit(1);
    }
    printf("\n\nAfter dropping A and D:\n");
    listpages();
    check_flags((uint64)stack_buf, stack_buf_size, 0, 0, 1);   // flags on this page may still be set if some other variables on it were accessed/written

    [[maybe_unused]] volatile char temp;
    for (int i = 0; i < stack_buf_size; ++i) {
        temp = stack_buf[i];
    }
    printf("\n\nAfter reading:\n");
    listpages();
    check_flags((uint64)stack_buf, stack_buf_size, 1, 0, 0);

    printf("TestArrayStack succeeded\n");
}

void TestArrayHeap()
{
    printf("\n\nTestArrayHeap started...\n");

    heap_buf = 0;

    printf("Initial state:\n");
    listpages();
    check_flags((uint64)heap_buf, alloc_size, -1, 1, 0);

    heap_buf = malloc(alloc_size);
    printf("\n\nAfter dynamic memory allocation:\n");
    listpages();
    check_flags((uint64)heap_buf, alloc_size, 0, 0, 0);

    memset(heap_buf, 1, alloc_size);
    printf("\n\nAfter writing to allocated memory:\n");
    listpages();
    check_flags((uint64)heap_buf, alloc_size, 1, 0, 0);

    switch (dropflags((uint64)heap_buf, alloc_size, PTE_A | PTE_D))
    {
    case -1:
        fprintf(2, "Error in dropflags: wrong flags\n");
        exit(1);
    case -2:
        fprintf(2, "Error in dropflags: bad address\n");
        exit(1);
    }
    printf("\n\nAfter dropping A and D:\n");
    listpages();
    check_flags((uint64)heap_buf, alloc_size, 0, 0, 0);

    [[maybe_unused]] volatile char temp;
    for (int i = 0; i < alloc_size; ++i) {
        temp = heap_buf[i];
    }
    printf("\n\nAfter reading:\n");
    listpages();
    check_flags((uint64)heap_buf, alloc_size, 1, 0, 0);

    free(heap_buf);
    printf("\n\nAfter freeing memory:\n");
    listpages();
    check_flags((uint64)heap_buf, alloc_size, 0, 0, 1);     // free may not clear flags

    printf("TestArrayHeap succeeded\n");
}

int main(int argc, char *argv[])
{
    TestVariableStack();
    TestVariableGlobal();
    TestArrayStack();
    TestArrayHeap();
    exit(0);
}