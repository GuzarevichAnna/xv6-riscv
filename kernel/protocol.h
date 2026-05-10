#define PROTOCOL_SYSCALLS       (1L << 0)
#define PROTOCOL_INTERRUPTS     (1L << 1)
#define PROTOCOL_PROCESS        (1L << 2)
#define PROTOCOL_EXEC           (1L << 3)

extern int cur_protocol_flags;
extern int protocol_timeout_ticks;
extern struct spinlock protocol_lock;

void protocol_timeout_check(void);

void protocol_init(void);
