//
//  HookSystemCall.c
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//

#include <libkern/libkern.h>
#include <mach/mach_types.h>

#include "HookSystemCall.h"
#include "debugInfo.h"

errno_t GetSystemTable()
{
    LOG(LOG_DEBUG, "enter");
    vm_address_t start_address, vm_size, end_address;
    struct sysent *syscall_start_ptr;
    uint64_t ret_value = getSystemCallAddress(&start_address, &vm_size);
    if (ret_value==-1)
    {
        LOG(LOG_ERROR, "Get system call entry address error, return code is %llu", ret_value);
        return KERN_FAILURE;
    }
    LOG(LOG_DEBUG, "Get system call entry address successfully, start address is %lu, size is %lu", start_address, vm_size);

    end_address = start_address+vm_size;

    while (start_address<end_address)
    {
        //test the struct sysent
        start_address++;
    }


    LOG(LOG_DEBUG, "leave");
    return KERN_SUCCESS;
}