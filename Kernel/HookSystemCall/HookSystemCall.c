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
#include "configure.h"

extern struct sysent_own* system_table;

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

int SetSystemCallHandle(void* function_handle, int syscall_no)
{
    LOG(LOG_DEBUG, "Enter");
    sy_call_t* original_func_ptr=NULL;

    if(system_table==NULL) return TROY_ERROR_INVALID_PARAMETER;

    CloseInterupt();
    DISABLE_WRITE_PROTECTION();

    original_func_ptr=system_table[syscall_no].sy_call;
    system_table[syscall_no].sy_call=(sy_call_t*)function_handle;

    ENABLE_WRITE_PROTECTION();
    RecorverInterupt();
    LOG(LOG_DEBUG, "Leave");
    return TROY_SUCCESS;
}

int CancelSystemCallHandle(void *original_func_ptr, void *current_func_ptr, int syscall_no)
{
    LOG(LOG_DEBUG,"Enter");



    LOG(LOG_DEBUG, "Leave");
    return 0;
}

int GetOriginalFunction(int syscall_no, sy_call_t **orginal_ptr)
{
    LOG(LOG_DEBUG, "Enter");

    if(system_table==NULL) return TROY_ERROR_INVALID_PARAMETER;

    CloseInterupt();
    DISABLE_WRITE_PROTECTION();
    *orginal_ptr=system_table[syscall_no].sy_call;
    ENABLE_WRITE_PROTECTION();
    RecorverInterupt();

    LOG(LOG_ERROR, "original_getdirentries_ptr address is %p", *orginal_ptr);
    LOG(LOG_DEBUG, "Leave");
    return TROY_SUCCESS;
}

