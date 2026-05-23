#define RING_BUFFER_SIZE PGSIZE * RING_BUFFER_PAGES_NUM

struct ring_buffer
{
    struct spinlock lock;

    int head;
    int tail;
    char data[RING_BUFFER_SIZE];

    // The buffer is empty if head == tail. If (tail + 1) % RING_BUFFER_SIZE == head then the buffer is full.
};

extern struct ring_buffer buf;

void ring_buffer_write_byte(char byte);

void ring_buffer_init(void);