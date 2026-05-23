#include "types.h"
#include "protocol.h"
#include "riscv.h"
#include "defs.h"

uint64 sys_set_protocol(void) {
    int flags = 0;
    int timeout = 0;
    argint(0, &flags);
    argint(1, &timeout);

    acquire(&protocol_lock);
    cur_protocol_flags = flags;
    if (timeout == 0) {
        // no timeout
        protocol_timeout_ticks = 0;
    } else {
        protocol_timeout_ticks = timeout;
    }
    release(&protocol_lock);

    return 0;
}
