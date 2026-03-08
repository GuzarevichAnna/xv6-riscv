#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int parse_two_numbers(char *input_buf, int *num1, int *num2)
{
    // replace whitespace with '\0'
    int input_idx = 0;
    while (input_idx < strlen(input_buf) + 1)
    {
        if (input_buf[input_idx] == ' ')
        {
            if (input_idx == 0) {
                // no first number encountered
                return -1;
            }
            input_buf[input_idx] = '\0';
            break;
        }
        if (input_idx == strlen(input_buf)) {
            // no whitespace encountered
            return -1;
        }

        ++input_idx;
    }

    *num1 = atoi(input_buf);
    // check if atoi resulted in error
    if (*num1 == 0 && (input_buf[0] != '0' || input_buf[1] != '\0')) {
        return -1;
    }

    *num2 = atoi(input_buf + input_idx + 1);
    // check if atoi resulted in error
    if (*num2 == 0 && (input_buf[input_idx + 1] != '0' || input_buf[input_idx + 2] != '\0')) {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    const int input_max_length = 22; // 10 (digits) + 1 (whitespace) + 10 (digits) + 1 ('\0')
    char input_buf[input_max_length];
    for (int i = 0; i < input_max_length; ++i)
    {
        int bytes_read = read(0, input_buf + i, 1);
        if (bytes_read == -1)
        {
            fprintf(2, "syscall read resulted in error\n");
            exit(1);
        }
        if (bytes_read == 0 || input_buf[i] == '\n')
        {
            input_buf[i] = '\0';
            break;
        }
    }
    input_buf[input_max_length - 1] = '\0';

    printf("Input: |%s|\n", input_buf);

    int num1;
    int num2;
    int parsing_result = parse_two_numbers(input_buf, &num1, &num2);
    if (parsing_result == -1)
    {
        fprintf(2, "parsing error\n");
        exit(1);
    }

    unsigned long long sum = add(num1, num2); // syscall
    if (sum == -1)
    {
        fprintf(2, "syscall add resulted in error\n");
        exit(1);
    }

    printf("%llu\n", sum);
    exit(0);
}