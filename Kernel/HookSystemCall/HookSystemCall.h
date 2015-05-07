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

//struct sysent_own * GetSystemTable();

#endif /* defined(__Troy__HookSystemCall__) */
