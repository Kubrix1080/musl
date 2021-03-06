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
#include "access.h"
#include "time.h"
#include "pipe.h"
#include "poll.h"
#include "fcntl.h"
#include "stat.h"
#include "getcwd.h"
#include "socket.h"
#include "bind.h"
#include "listen.h"
#include "accept.h"
#include "ioctl.h"
#include "sockopt.h"
#include "getsockname.h"
#include "getpeername.h"
#include "futex.h"
#include "getdents.h"

#include "pthread_impl.h"

#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>

#include <errno.h>

extern int __vita_external_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6) __attribute__((weak));

int __vita_syscall_interp(int n, int r1, int r2, int r3, int r4, int r5, int r6)
{
    //if (n != SYS_poll && n != SYS_clock_gettime)
    //    sceClibPrintf("musl: syscall called: %i: %s\n", n, __lookup_syscall_name(n));

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
    case SYS_set_tid_address:
        __pthread_self()->clear_child_tid = (int *)r1;
        return sceKernelGetThreadId();
    case SYS_gettid:
        return sceKernelGetThreadId();
    case SYS_tkill:
        sceClibPrintf("got tkill for: %i %i\n", r1, r2);
        return 0;
    case SYS_brk:
        return (int)__vita_brk((void *)r1);
    case SYS_mmap2:
        return (int)__vita_mmap((void *)r1, r2, r3, r4, r5, r6);
    case SYS_munmap:
        return __vita_munmap((void *)r1, r2);
    case SYS_open:
        return __vita_open((const char *)r1, r2);
    case SYS_read:
        return __vita_read(r1, (void *)r2, r3);
    case SYS_readv:
        return __vita_readv(r1, (const struct iovec *)r2, r3);
    case SYS_writev:
        return __vita_writev(r1, (const struct iovec *)r2, r3);
    case SYS_close:
        return __vita_close(r1);
    case SYS__llseek:
        return __vita__llseek(r1, r2, r3, (off_t *)r4, r5);
    case SYS_ioctl:
        return __vita_ioctl(r1, r2, r3);
    case SYS_access:
        return __vita_access((const char *)r1, r2);
    case SYS_clock_gettime:
        return __vita_clock_gettime(r1, (struct timespec *)r2);
    case SYS_pipe:
        return __vita_pipe2((int *)r1, 0);
    case SYS_pipe2:
        return __vita_pipe2((int *)r1, r2);
    case SYS_poll:
        return __vita_poll((struct pollfd *)r1, r2, r3);
    case SYS_fcntl64:
        return __vita_fcntl64(r1, r2, r3);
    case SYS_lstat64:
        return __vita_stat64((const char *)r1, (struct stat *)r2);
    case SYS_stat64:
        return __vita_stat64((const char *)r1, (struct stat *)r2);
    case SYS_getcwd:
        return __vita_getcwd((char *)r1, r2);
    case SYS_socket:
        return __vita_socket(r1, r2, r3);
    case SYS_bind:
        return __vita_bind(r1, (const struct sockaddr *)r2, r3);
    case SYS_listen:
        return __vita_listen(r1, r2);
    case SYS_accept:
        return __vita_accept4(r1, (struct sockaddr * )r2, (socklen_t *)r3, 0);
    case SYS_accept4:
        return __vita_accept4(r1, (struct sockaddr * )r2, (socklen_t *)r3, r4);
    case SYS_write:
        return __vita_write(r1, (const void *)r2, r3);
    case SYS_getsockopt:
        return __vita_getsockopt(r1, r2, r3, (void *)r4, (socklen_t *)r5);
    case SYS_setsockopt:
        return __vita_setsockopt(r1, r2, r3, (const void *)r4, (socklen_t)r5);
    case SYS_getsockname:
        return __vita_getsockname(r1, (struct sockaddr *)r2, (socklen_t *)r3);
    case SYS_getpeername:
        return __vita_getpeername(r1, (struct sockaddr *)r2, (socklen_t *)r3);
    case SYS_futex:
        return __vita_futex((int *)r1, r2, r3, (const struct timespec *)r4, (int *)r5, r6);
    case SYS_getdents64:
        return __vita_getdents(r1, (struct dirent *)r2, r3);
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
