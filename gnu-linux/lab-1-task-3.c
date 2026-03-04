#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

const int io_chunk_size = 1024;

int main(int argc, char *argv[])
{
    int p[2];
    int pipe_ret_value = pipe(p);
    if (pipe_ret_value < 0) {
        printf("Error creating a pipe\n");
        exit(1);
    }

    int pid = fork();
    if (pid < 0)
    {
        printf("Fork error\n");
        exit(1);
    }
    else if (pid > 0)
    {
        // in parent
        int close_ret_value = close(p[0]);
        if (close_ret_value < 0) {
            printf("Error closing read end of pipe in parent\n");
            exit(1);
        }

        // copy command line arguments to local buffer
        char buf[io_chunk_size];
        int input_len = 0;
        for (int i = 0; i < argc; ++i)
        {
            memcpy(buf + input_len, argv[i], strlen(argv[i]));
            input_len += strlen(argv[i]);
            memcpy(buf + input_len, "\n", 1);
            ++input_len;
        }

        int bytes_written_total = 0;
        while (bytes_written_total < input_len)
        {
            int bytes_written = write(p[1], buf + bytes_written_total, MIN(io_chunk_size, input_len - bytes_written_total));
            if (bytes_written <= 0)
            {
                printf("Error in write in parent\n");
                exit(1);
            }
            bytes_written_total += bytes_written;
        }

        close_ret_value = close(p[1]);
        if (close_ret_value < 0) {
            printf("Error closing write end of pipe in parent\n");
            exit(1);
        }

        int pid = wait(0);
        if (pid == -1)
        {
            printf("Error: parent has no children?!\n");
            exit(1);
        }
        exit(0);
    }
    else
    {
        // in child
        int close_ret_value = close(p[1]);
        if (close_ret_value < 0) {
            printf("Error closing write end of pipe in child\n");
            exit(1);
        }

        char buf[io_chunk_size];
        while (1)
        {
            int bytes_read = read(p[0], buf, sizeof(buf));
            if (bytes_read < 0) {
                printf("Error in read\n");
                exit(1);
            }
            if (bytes_read == 0)
            {
                break;
            }
            
            int bytes_written_total = 0;
            while (bytes_written_total < bytes_read)
            {
                int bytes_written = write(1, buf + bytes_written_total, bytes_read - bytes_written_total);
                if (bytes_written <= 0) {
                    printf("Error in write in child\n");
                    exit(1);
                }
                bytes_written_total += bytes_written;
            }
        }

        close_ret_value = close(p[0]);
        if (close_ret_value < 0) {
            printf("Error closing read end of pipe in child\n");
            exit(1);
        }

        exit(0);
    }
}