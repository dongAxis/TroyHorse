//
//  KernelInfo.c
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//
//#include <sys/malloc.h>
#include <mach/mach_types.h>
#include <mach-o/loader.h>
#include <sys/types.h>
#include <sys/kernel_types.h>
#include <sys/systm.h>
#include <sys/unistd.h>

#include "KernelInfo.h"
#include "debugInfo.h"

static mach_vm_address_t GetInteruptTableAddr()
{
    LOG(LOG_DEBUG, "enter");
    uint8_t idtr[10];
    bzero(idtr, sizeof(uint8_t)*10);
    __asm__ __volatile__("sidt %0":"=m"(idtr));
    
    LOG(LOG_DEBUG, "leave");
    return *(mach_vm_address_t*)(&idtr[2]);
}

static uint16_t GetInteruptTableSize()
{
    uint8_t idtr[10];
    bzero(idtr, sizeof(uint8_t)*10);
    __asm__ __volatile__("side %0":"=m"(idtr));
    
    return *(uint16_t*)(&idtr[0]);
}

static mach_vm_address_t GetInt80Address()
{
    LOG(LOG_DEBUG, "enter");
    mach_vm_address_t idtr_addr = GetInteruptTableAddr();   //get the interput table's start address
    mach_vm_address_t idtr_80_addr = idtr_addr+sizeof(struct idt_descriptor)*0x80;  //get the int 80 start address
    struct idt_descriptor *idtr_int80_ptr = (struct idt_descriptor*)idtr_80_addr;
    
    uint64_t hight_addr = (uint64_t)idtr_int80_ptr->hight<<32;
    uint32_t middle_addr = (uint32_t)idtr_int80_ptr->middle<<16;
    uint64_t int80_vm_addr = hight_addr + (uint64_t)middle_addr + (uint64_t)idtr_int80_ptr->low;
    
    LOG(LOG_DEBUG, "leave");
    return (mach_vm_address_t)int80_vm_addr;
}

static mach_vm_address_t getKernelHeader()
{
    mach_vm_address_t kernel_tmp_address = GetInt80Address();
    
    while(kernel_tmp_address>0)
    {
        struct mach_header_64 *header = (struct mach_header_64*)kernel_tmp_address;
        if(header->magic==MH_MAGIC_64)
        {
            struct segment_command_64 *text_segment = (struct segment_command_64 *)++header;
            if(strcmp(text_segment->segname, SEG_TEXT)==0)
            {
                return kernel_tmp_address;
            }
        }
        kernel_tmp_address--;
    }
    
    return 0;
}

static mach_vm_address_t getSystmEntryAddr()
{
    mach_vm_address_t kernel_address = getKernelHeader();
    if(kernel_address==0) return 0;
    struct mach_header_64 *header = (struct mach_header_64*)kernel_address;
    uint32_t ncmds = header->ncmds;
    mach_vm_address_t segment_start_address=(mach_vm_address_t)++header;    //skip the header
    
    for(int i=0; i<ncmds; i++)
    {
        struct segment_command_64 *segment_64 =
                (struct segment_command_64*)(segment_start_address+i*sizeof(struct segment_command_64));
        if(strcmp(segment_64->segname,SEG_DATA)==0)
        {
            return (mach_vm_address_t)segment_64;
        }
    }
    
    return 0;
}

mach_vm_address_t getSystemCallAddress(vm_address_t *start_address)
{
    mach_vm_address_t system_call_address = getSystmEntryAddr();
    if(system_call_address==0) return 0;
    
    struct segment_command_64 *data_segment = (struct segment_command_64*)system_call_address;
    data_segment->vmaddr
}