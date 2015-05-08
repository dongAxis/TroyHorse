//
//  Control.h
//  Troy
//
//  Created by Axis on 5/7/15.
//  Copyright (c) 2015 Axis. All rights reserved.
//

#ifndef __Troy__Control__
#define __Troy__Control__

#include <mach/mach_types.h>
#include <sys/systm.h>
#include <miscfs/devfs/devfs.h>
#include <libkern/libkern.h>
#include <sys/conf.h>

#define nullopen    (d_open_t *)&nulldev;
#define nullclose   (d_close_t *)&nulldev;
#define nullread	(d_read_t *)&nulldev
#define nullwrite	(d_write_t *)&nulldev
#define nullselect	(d_select_t *)&nulldev
#define nullstop	(d_stop_t *)&nulldev
#define nullreset	(d_reset_t *)&nulldev

extern struct cdevsw troy_ops;

errno_t init_troy_components();
errno_t destroy_troy_component();

#endif /* defined(__Troy__Control__) */
