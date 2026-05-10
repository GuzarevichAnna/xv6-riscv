#include "types.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "protocol.h"

int cur_protocol_flags = 0; // on start, no events are protocolled
int protocol_timeout_ticks = 0; // no timeout by default
struct spinlock protocol_lock;

void
protocol_timeout_check(void)
{
    acquire(&protocol_lock);
    if (protocol_timeout_ticks > 0) {
        --protocol_timeout_ticks;
        if (protocol_timeout_ticks == 0) {
            cur_protocol_flags = 0;
        }
    }
    release(&protocol_lock);
}

void
protocol_init(void)
{
    initlock(&protocol_lock, "protocol");
}