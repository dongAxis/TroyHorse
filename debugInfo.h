//
//  debugInfo.h
//  Troy
//
//  Created by Axis on 15/5/6.
//  Copyright (c) 2015年 Axis. All rights reserved.
//

#ifndef Troy_debugInfo_h
#define Troy_debugInfo_h

#define LOG_NONE	0
#define LOG_EMER	1
#define LOG_ALERT	2
#define LOG_CRIT	3
#define LOG_ERROR	4
#define LOG_WARN	5
#define LOG_NOTICE	6
#define LOG_INFO	7
#define LOG_DEBUG	8

static char debug_text_info[][100] = {
    {"LOG_NONE"},
    {"LOG_EMER"},
    {"LOG_ALERT"},
    {"LOG_CRIT"},
    {"LOG_ERROR"},
    {"LOG_WARN"},
    {"LOG_NOTICE"},
    {"LOG_INFO"},
    {"LOG_DEBUG"}
};

#define LOG(debug_level ,args...)                                                    \
        do {                                                                         \
            printf("[TROY][%s] %s:", debug_text_info[debug_level],__FUNCTION__);     \
            printf(args);                                                            \
            printf("\n");                                                            \
        }while(0)


#endif
