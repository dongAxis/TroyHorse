//
//  debugInfo.h
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//

#include <mach/mach_types.h>
#include <libkern/libkern.h>
#include <sys/conf.h>
#include <sys/ioctl.h>
#include <miscfs/devfs/devfs.h>
#include <mach-o/loader.h>

#include "debugInfo.h"
#include "KernelInfo.h"
#include "configure.h"

kern_return_t troy_start(kmod_info_t * ki, void *d);
kern_return_t troy_stop(kmod_info_t *ki, void *d);

kern_return_t troy_start(kmod_info_t * ki, void *d)
{
    LOG(LOG_DEBUG, "start");

//    mach_vm_address_t addr =  getKernelHeader();
//    if(addr==0)
//    {
//        goto failed;
//    }
//    struct mach_header_64 *header = (struct mach_header_64*)addr;
//    LOG(LOG_DEBUG, "magic=%x", header->magic);
//    LOG(LOG_DEBUG, "cpusubtype=%x", header->cpusubtype);
//    LOG(LOG_DEBUG, "filetype=%x", header->filetype);
    
    mach_vm_address_t addr2 = getSystmEntryAddr();
    struct segment_command_64 *seg_data = (struct segment_command_64*)addr2;
    LOG(LOG_DEBUG, "seg name is %s", seg_data->segname);
    
    return KERN_SUCCESS;
failed:
    return KERN_FAILURE;
}

kern_return_t troy_stop(kmod_info_t *ki, void *d)
{
    printf("[TROY] %s\n",__FUNCTION__);
    printf("troy_stop stop\n");
    
    return KERN_SUCCESS;
}
