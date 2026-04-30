#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

#define Read_RTC_LOW (*((volatile uint32*)(RTC_LOW)))
#define Read_RTC_HIGH (*((volatile uint32*)(RTC_HIGH)))

int64 get_time() {
    intr_off();
    int64 rtc = Read_RTC_LOW;
    rtc |= ((int64)Read_RTC_HIGH << 32);
    intr_on();
    return rtc;
}

int64 sys_get_time(void) {
    int64 time = get_time();
    return time;
}
