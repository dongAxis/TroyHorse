//
//  HookSystemCall.h
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//

#ifndef __Troy__HookSystemCall__
#define __Troy__HookSystemCall__

#include "KernelInfo.h"

#define DISABLE_WRITE_PROTECTION() asm volatile ( \
    "mov %cr0,%rax\n" \
    "and $0xfffffffffffeffff,%rax\n" \
    "mov %rax,%cr0" \
)
// re-enable write protection by re-setting wp flag in ctrl reg cr0
#define ENABLE_WRITE_PROTECTION() asm volatile ( \
    "mov %cr0,%rax\n" \
    "or $0x10000,%rax\n" \
    "mov %rax,%cr0\n" \
)

/*
 for Yousemite.
 */
typedef int32_t  sy_call_t(struct proc*, void*, int*);
typedef void sy_munge_t(const void*,void*);

struct sysent_own {		/* system call table */
    sy_call_t   *sy_call;
    sy_munge_t  *sy_arg_munge32;
    int32_t     sy_return_type;
    int16_t     sy_narg;
    uint16_t    sy_arg_bytes;
};

int GetOriginalFunction(int syscall_no, mach_vm_address_t *orginal_ptr_addr);
int SetSystemCallHandle(void* function_handle, int syscall_no);
struct sysent_own* GetSystemTable();

#endif /* defined(__Troy__HookSystemCall__) */
