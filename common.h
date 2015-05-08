//
//  common.h
//  Troy
//
//  Created by Axis on 5/8/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#ifndef __Troy__common__
#define __Troy__common__

#include <mach/mach_types.h>
#include <libkern/libkern.h>

#define LCK_GRP_NAME "com.axis.troy.lockgrp"

errno_t alloc_kext_lock();
void free_kext_lock();

#endif /* defined(__Troy__common__) */
