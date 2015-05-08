//
//  common.c
//  Troy
//
//  Created by Axis on 5/8/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#include "common.h"

#include <string.h>
#include <sys/proc.h>
#include <kern/kern_types.h>
#include <miscfs/devfs/devfs.h>

#include "Control.h"
#include "configure.h"
#include "debugInfo.h"

extern lck_grp_t *lck_grp;
extern lck_grp_attr_t *lck_grp_attr;

errno_t alloc_kext_lock()
{
    LOG(LOG_DEBUG, "Enter");
    lck_grp_attr = lck_grp_attr_alloc_init();
    if(lck_grp_attr==NULL)
    {
        LOG(LOG_ERROR, "the lock group attribute allocate memory failed");
        return KERN_FAILURE;
    }
    lck_grp_attr_setstat(lck_grp_attr);

    lck_grp = lck_grp_alloc_init(LCK_GRP_NAME, lck_grp_attr);
    if(lck_grp==NULL)
    {
        LOG(LOG_ERROR, "the lock group allocate memory failed");
        return KERN_FAILURE;
    }

    //todo: allocate the memory for other lock

    LOG(LOG_DEBUG, "Leave");
    return KERN_SUCCESS;
}

void free_kext_lock()
{
    //todo: free other lock's memory

    if(lck_grp)     //free lock group
    {
        lck_grp_free(lck_grp);
        lck_grp=NULL;
    }

    if(lck_grp_attr)    //free attr
    {
        lck_grp_attr_free(lck_grp_attr);
        lck_grp_attr=NULL;
    }
}