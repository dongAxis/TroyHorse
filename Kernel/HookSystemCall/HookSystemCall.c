//
//  HookSystemCall.c
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//
#include <sys/syscall.h>
#include <libkern/libkern.h>
#include <mach/mach_types.h>

#include "HookSystemCall.h"
#include "debugInfo.h"


struct sysent_own* GetSystemTable()
{
    LOG(LOG_DEBUG, "enter");
    uint64_t start_address=0, vm_size=0, end_address=0;
    uint64_t ret_value = GetSystemCallAddress(&start_address, &vm_size);
    if (ret_value==-1)
    {
        LOG(LOG_ERROR, "Get system call entry address error, return code is %llu", ret_value);
        return NULL;
    }
    LOG(LOG_DEBUG, "Get system call entry address successfully, start address is %llu, size is %llu", start_address, vm_size);

    end_address = start_address+vm_size;

    while (start_address<end_address)
    {
        struct sysent_own *syscall_table_ptr=(struct sysent_own*)start_address;

        LOG(LOG_ERROR, "syscall_table_ptr[SYS_syscall].sy_narg=%d", syscall_table_ptr[SYS_syscall].sy_narg);
        LOG(LOG_ERROR, "syscall_table_ptr[SYS_exit].sy_narg=%d", syscall_table_ptr[SYS_exit].sy_narg);
        LOG(LOG_ERROR, "syscall_table_ptr[SYS_fork].sy_narg=%d", syscall_table_ptr[SYS_fork].sy_narg);
        LOG(LOG_ERROR, "syscall_table_ptr[SYS_read].sy_narg=%d", syscall_table_ptr[SYS_read].sy_narg);
        LOG(LOG_ERROR, "syscall_table_ptr[SYS_ptrace].sy_narg=%d", syscall_table_ptr[SYS_ptrace].sy_narg);

        if (syscall_table_ptr!=NULL &&
            syscall_table_ptr[SYS_syscall].sy_narg == 0 &&
            syscall_table_ptr[SYS_exit].sy_narg == 1  &&
            syscall_table_ptr[SYS_fork].sy_narg == 0 &&
            syscall_table_ptr[SYS_read].sy_narg == 3 &&
            syscall_table_ptr[SYS_wait4].sy_narg == 4 &&
            syscall_table_ptr[SYS_ptrace].sy_narg == 4)
        {
            LOG(LOG_DEBUG, "Get system call table successfully. new");
            return syscall_table_ptr;
        }
        //test the struct sysent
        start_address++;
    }

    LOG(LOG_DEBUG, "leave new");
    return NULL;
}