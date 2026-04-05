#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char digit_to_byte(char digit)
{
    if (digit < '9')
    {
        return digit - '0';
    }
    return digit - 'A' + 10;
}

void fill_buf(char *buf, char *string, int bytes_count)
{
    if (strlen(string) & 1)
    {
        // interpret last symbol as low half of last byte
        buf[bytes_count - 1] = digit_to_byte(string[strlen(string) - 1]);
    }
    for (int i = 0; i < strlen(string) - 1; i += 2)
    {
        buf[i / 2] = digit_to_byte(string[i]);
        buf[i / 2] = buf[i / 2] << 4;
        buf[i / 2] |= digit_to_byte(string[i + 1]);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Wrong number of arguments.\n");
        exit(1);
    }

    int fd;

    if ((fd = open(argv[2], O_WRONLY)) < 0)
    {
        fprintf(2, "hexdump: cannot open %s\n", argv[2]);
        exit(1);
    }

    int bytes_to_write = (strlen(argv[1]) + 1) / 2;
    char buf[bytes_to_write];

    fill_buf(buf, argv[1], bytes_to_write);

    if (write(fd, buf, bytes_to_write) != bytes_to_write)
    {
        fprintf(2, "hexdump: write error\n");
        exit(1);
    }

    if (close(fd) < 0)
    {
        fprintf(2, "hexdump: close error\n");
        exit(1);
    }

    exit(0);
}