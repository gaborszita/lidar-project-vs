/* 
* Copyright (C) 2017-2020 Gabor Szita
* 
* This file is part of lidar project.
*
* Lidar project is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 3 as published by
* the Free Software Foundation.
*
* Lidar project is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with lidar project.  If not, see https://www.gnu.org/licenses/ 
*/ 

#ifndef _OPENCL_INIT_H
#define _OPENCL_INIT_H

#include "opencl_config.h"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class OpenCLInit
{
private:
	cl_int err;
	//cl_context* testFunc();
	cl_device_id* devices;

public:
	cl_context context;
	cl_command_queue gpu_queue;
	cl_device_id* gpu_device;

public:
	OpenCLInit();
	~OpenCLInit();

private:
	void displayPlatformInfo(cl_uint num_platforms, cl_platform_id* platforms);
	void displayDeviceInfo(cl_uint num_devices, cl_device_id* devices);
};

#endif