#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define Read_RTC_LOW (*((volatile uint32*)(RTC_LOW)))
#define Read_RTC_HIGH (*((volatile uint32*)(RTC_HIGH)))

uint64 get_time() {
    uint64 rtc = Read_RTC_LOW;
    rtc |= ((uint64)Read_RTC_HIGH << 32);
    return rtc;
}

uint64 sys_get_time(void) {
    uint64 time = get_time();
    return time;
}
