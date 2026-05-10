#include "param.h"
#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "ring_buffer.h"

struct ring_buffer buf;

void ring_buffer_write_byte(char byte)
{
    acquire(&buf.lock);

    buf.data[buf.tail] = byte;
    if ((buf.tail + 1) % RING_BUFFER_SIZE == buf.head)
    {
        // buffer is full - move both head and tail
        buf.head = buf.tail = (buf.tail + 1) % RING_BUFFER_SIZE;
    }
    else
    {
        // buffer is not full, old data is not overwritten - move only tail
        buf.tail = (buf.tail + 1) % RING_BUFFER_SIZE;
    }
    release(&buf.lock);
}

void ring_buffer_init(void)
{
    buf.head = 0;
    buf.tail = 0;

    initlock(&buf.lock, "ring buffer");
}
