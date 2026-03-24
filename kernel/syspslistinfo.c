#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

extern struct spinlock wait_lock;
extern struct proc proc[NPROC];

uint64 sys_ps_listinfo(void)
{
    struct procinfo *plist;
    int lim;
    argaddr(0, (uint64 *)&plist);
    argint(1, &lim);

    int processes_count = 0;
    struct procinfo kernel_local_procinfo;

    acquire(&wait_lock);
    for (int i = 0; i < NPROC; ++i)
    {
        acquire(&proc[i].lock);
        if (proc[i].state == USED || proc[i].state == SLEEPING || proc[i].state == RUNNABLE || proc[i].state == RUNNING)
        {
            ++processes_count;
            if (plist!= 0 && processes_count > lim)
            {
                release(&proc[i].lock);
                release(&wait_lock);
                return -1;
            }
            if (plist != 0)
            {
                kernel_local_procinfo.pid = proc[i].pid;
                safestrcpy(kernel_local_procinfo.name, proc[i].name, sizeof(proc[i].name));
                kernel_local_procinfo.state = proc[i].state;
                struct proc *parent = proc[i].parent;
                release(&proc[i].lock);
                if (parent == 0)
                {
                    kernel_local_procinfo.parent_pid = -1;
                }
                else
                {
                    acquire(&parent->lock);
                    kernel_local_procinfo.parent_pid = parent->pid;
                    safestrcpy(kernel_local_procinfo.parent_name, parent->name, sizeof(parent->name));
                    release(&parent->lock);
                }

                if (copyout(myproc()->pagetable, (uint64)plist + (processes_count - 1) * sizeof(struct procinfo), (char *)&kernel_local_procinfo, sizeof(struct procinfo)) == -1)
                {
                    release(&wait_lock);
                    return -2;
                }
            }
            else
            {
                release(&proc[i].lock);
            }
        }
        else
        {
            release(&proc[i].lock);
        }
    }

    release(&wait_lock);
    return processes_count;
}