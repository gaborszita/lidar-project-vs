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