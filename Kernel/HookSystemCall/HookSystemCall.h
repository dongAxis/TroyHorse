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
typedef	int32_t	sy_call_t(struct proc *, void *, int *);
typedef	void	sy_munge_t(void *);

struct sysent {		/* system call table */
    sy_call_t	*sy_call;	/* implementing function */
    sy_munge_t	*sy_arg_munge32; /* system call arguments munger for 32-bit process */
    int32_t		sy_return_type; /* system call return types */
    int16_t		sy_narg;	/* number of args */
    uint16_t	sy_arg_bytes;	/* Total size of arguments in bytes for
                                 * 32-bit system calls
                                 */
};

errno_t GetSystemTable();

#endif /* defined(__Troy__HookSystemCall__) */
