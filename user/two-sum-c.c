#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// returns the number of digits parsed
int parse_one_number(const char *input_buf, int *num)
{
    char temp[11]; // 10 digits + '\0'
    int input_idx = 0;
    int temp_idx = 0;
    while (input_buf[input_idx] != ' ' && input_buf[input_idx] != '\0')
    {
        temp[temp_idx++] = input_buf[input_idx++];
    }
    temp[temp_idx] = '\0';
    if (temp_idx == 0)
    {
        return -1;
    }

    *num = atoi(temp);
    // check if atoi resulted in error
    if (*num == 0 && temp[0] != '0' && (temp[1] != '\0' || temp[1] != ' ')) {
        return -1;
    }

    return input_idx;
}

int parse_two_numbers(const char *input_buf, int *num1, int *num2)
{
    int input_idx = 0;

    int first_number_parsing_result = parse_one_number(input_buf, num1);
    if (first_number_parsing_result < 0)
    {
        return -1;
    }

    input_idx += first_number_parsing_result;
    input_idx++; // skip whitespace

    int second_number_parsing_result = parse_one_number(input_buf + input_idx, num2);
    if (second_number_parsing_result < 0)
    {
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