#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char hex[3];

char *byte_to_hex(char byte)
{
    hex[0] = byte / 16;
    if (hex[0] <= 9)
    {
        hex[0] += '0';
    }
    else
    {
        hex[0] += 'A' - 10;
    }
    hex[1] = byte % 16;
    if (hex[1] <= 9)
    {
        hex[1] += '0';
    }
    else
    {
        hex[1] += 'A' - 10;
    }
    hex[2] = '\0';
    return hex;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Wrong number of arguments.\n");
        exit(1);
    }

    int bytes_to_read = atoi(argv[1]);
    char buf[bytes_to_read];
    int fd;

    if ((fd = open(argv[2], O_RDONLY)) < 0)
    {
        fprintf(2, "hexdump: cannot open %s\n", argv[2]);
        exit(1);
    }

    int total_bytes_read = 0;
    while (total_bytes_read < bytes_to_read)
    {
        int bytes_read = read(fd, buf, sizeof(buf));
        if (bytes_read < 0)
        {
            fprintf(2, "hexdump: read error\n");
            exit(1);
        }

        if (bytes_read == 0)
        {
            break;
        }

        total_bytes_read += bytes_read;

        for (int i = 0; i < bytes_read; ++i)
        {
            char *hex = byte_to_hex(buf[i]);
            printf("%s ", hex);
        }
    }
    printf("\n");

    if (close(fd) < 0)
    {
        fprintf(2, "hexdump: close error\n");
        exit(1);
    }
    exit(0);
}