#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/protocol.h"

int main(int argc, char *argv[])
{
  int flags = 0;
  int timeout = 0;
  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "syscall") == 0)
    {
      flags |= PROTOCOL_SYSCALLS;
    }
    else if (strcmp(argv[i], "interrupt") == 0)
    {
      flags |= PROTOCOL_INTERRUPTS;
    }
    else if (strcmp(argv[i], "process") == 0)
    {
      flags |= PROTOCOL_PROCESS;
    }
    else if (strcmp(argv[i], "exec") == 0)
    {
      flags |= PROTOCOL_EXEC;
    }
    else if (strcmp(argv[i], "-t") == 0)
    {
      if (i + 1 >= argc)
      {
        fprintf(2, "Invalid arguments. Usage: set_protocol [syscall] [interrupt] [process] [exec] [-t ticks]\n");
        exit(1);
      }
      timeout = atoi(argv[i + 1]);
      ++i; // skip the number we have just parsed
    }
    else
    {
      fprintf(2, "Invalid arguments. Usage: set_protocol [syscall] [interrupt] [process] [exec] [-t ticks]\n");
      exit(1);
    }
  }
  
  set_protocol(flags, timeout);
  exit(0);
}