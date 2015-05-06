//
//  KernelInfo.h
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//

#ifndef __Troy__KernelInfo__
#define __Troy__KernelInfo__

#include <sys/types.h>
#include <libkern/libkern.h>

#include "debugInfo.h"

/*
 * This is idt descriptor.
 * the following is used for 64-bit machine, and little-endine
 */
struct idt_descriptor
{
    uint16_t low;
    uint16_t selector;
    uint8_t reserver1;
    uint8_t flags;
    uint16_t middle;
    uint32_t hight;
    uint32_t reserver2;
};


#endif /* defined(__Troy__KernelInfo__) */
