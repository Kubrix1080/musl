#include "syscall.h"
#include "syscall_interface.h"

#include "tls.h"
#include "brk.h"
#include "mmap.h"

#include "open.h"
#include "read.h"
#include "write.h"
#include "close.h"
#include "seek.h"

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>

#include <errno.h>

extern int __vita_external_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6) __attribute__((weak));

int __vita_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6)
{
    switch (n)
    {
    case SYS_exit:
        return sceKernelExitDeleteThread(r1);
    case SYS_exit_group:
        return sceKernelExitProcess(r1);
    case SYS_geteuid:
    case SYS_getuid:
    case SYS_getegid32:
    case SYS_getpid:
        return sceKernelGetProcessId();
    case SYS_gettid:
        return sceKernelGetThreadId();
    case SYS_tkill:
        sceClibPrintf("got tkill for: %i %i\n", r1, r2);
        return 0;
    case SYS_brk:
        return (int)__vita_brk((void *)r1);
    case SYS_mmap2:
        return (int)__vita_mmap((void *)r1, r2, r3, r4, r5, r6);
    case SYS_open:
        return __vita_open((const char *)r1, r2);
    case SYS_read:
        return __vita_read(r1, (void *)r2, r3);
    case SYS_writev:
        return __vita_writev(r1, (const struct iovec *)r2, r3);
    case SYS_close:
        return __vita_close(r1);
    case SYS__llseek:
        return __vita__llseek(r1, r2, r3, (off_t *)r4, r5);
    case __NR_ARM_set_tls:
        return __vita_set_tls((void *)r1);
    default:
        if (__vita_external_syscall_interp)
            return __vita_external_syscall_interp(n, r1, r2, r3, r4, r5, r6);
        else
            sceClibPrintf("musl: unhandled syscall called: %i: %s\n", n, __lookup_syscall_name(n));
    }

    return -ENOSYS;
}