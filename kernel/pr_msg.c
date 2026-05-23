#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

// lock to avoid interleaving concurrent pr_msg's.
static struct
{
    struct spinlock lock;
} pr_m;

static char digits[] = "0123456789abcdef";

static void
printint(long long xx, int base, int sign)
{
    char buf[20];
    int i;
    unsigned long long x;

    if (sign && (sign = (xx < 0)))
        x = -xx;
    else
        x = xx;

    i = 0;
    do
    {
        buf[i++] = digits[x % base];
    } while ((x /= base) != 0);

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        ring_buffer_write_byte(buf[i]);
}

static void
printptr(uint64 x)
{
    int i;
    ring_buffer_write_byte('0');
    ring_buffer_write_byte('x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
        ring_buffer_write_byte(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

int pr_msg(const char *fmt, ...)
{
    va_list ap;
    int i, cx, c0, c1, c2;
    char *s;

    acquire(&pr_m.lock);

    ring_buffer_write_byte('[');
    acquire(&tickslock);
    uint ticks_value = ticks;
    release(&tickslock);
    printint(ticks_value, 10, 0);
    ring_buffer_write_byte(']');
    ring_buffer_write_byte(' ');

    va_start(ap, fmt);
    for (i = 0; (cx = fmt[i] & 0xff) != 0; i++)
    {
        if (cx != '%')
        {
            ring_buffer_write_byte(cx);
            continue;
        }
        i++;
        c0 = fmt[i + 0] & 0xff;
        c1 = c2 = 0;
        if (c0)
            c1 = fmt[i + 1] & 0xff;
        if (c1)
            c2 = fmt[i + 2] & 0xff;
        if (c0 == 'd')
        {
            printint(va_arg(ap, int), 10, 1);
        }
        else if (c0 == 'l' && c1 == 'd')
        {
            printint(va_arg(ap, uint64), 10, 1);
            i += 1;
        }
        else if (c0 == 'l' && c1 == 'l' && c2 == 'd')
        {
            printint(va_arg(ap, uint64), 10, 1);
            i += 2;
        }
        else if (c0 == 'u')
        {
            printint(va_arg(ap, uint32), 10, 0);
        }
        else if (c0 == 'l' && c1 == 'u')
        {
            printint(va_arg(ap, uint64), 10, 0);
            i += 1;
        }
        else if (c0 == 'l' && c1 == 'l' && c2 == 'u')
        {
            printint(va_arg(ap, uint64), 10, 0);
            i += 2;
        }
        else if (c0 == 'x')
        {
            printint(va_arg(ap, uint32), 16, 0);
        }
        else if (c0 == 'l' && c1 == 'x')
        {
            printint(va_arg(ap, uint64), 16, 0);
            i += 1;
        }
        else if (c0 == 'l' && c1 == 'l' && c2 == 'x')
        {
            printint(va_arg(ap, uint64), 16, 0);
            i += 2;
        }
        else if (c0 == 'p')
        {
            printptr(va_arg(ap, uint64));
        }
        else if (c0 == 'c')
        {
            ring_buffer_write_byte(va_arg(ap, uint));
        }
        else if (c0 == 's')
        {
            if ((s = va_arg(ap, char *)) == 0)
                s = "(null)";
            for (; *s; s++)
                ring_buffer_write_byte(*s);
        }
        else if (c0 == '%')
        {
            ring_buffer_write_byte('%');
        }
        else if (c0 == 0)
        {
            break;
        }
        else
        {
            // Print unknown % sequence to draw attention.
            ring_buffer_write_byte('%');
            ring_buffer_write_byte(c0);
        }
    }
    va_end(ap);

    ring_buffer_write_byte('\n');

    release(&pr_m.lock);

    return 0;
}

void pr_msg_init(void)
{
    initlock(&pr_m.lock, "print_msg");
}
