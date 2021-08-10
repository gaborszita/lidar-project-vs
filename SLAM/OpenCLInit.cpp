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

#include "OpenCLInit.h"
#include <stdio.h>
#include <Windows.h>
#define _CRT_SECURE_NO_WARNINGS

OpenCLInit::~OpenCLInit()
{
	clReleaseContext(context);
	clReleaseCommandQueue(gpu_queue);
	free(devices); // free devices to save memory
}

OpenCLInit::OpenCLInit()
{
	cl_platform_id* platforms;
	cl_uint num_platforms, num_devices;
	int selectedPlatform, selectedDevice;

	// get platforms
	err = clGetPlatformIDs(0, NULL, &num_platforms);
	if (err < 0)
	{
		printf("Couldn't find any platforms.");
		system("pause");
		exit(1);
	}
	platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * num_platforms);
	clGetPlatformIDs(num_platforms, platforms, NULL);
	displayPlatformInfo(num_platforms, platforms); // display platforms' information on screen

	selectedPlatform = 0; // select platform 0

	// get devices on the selected platform
	err = clGetDeviceIDs(platforms[selectedPlatform], CL_DEVICE_TYPE_GPU, 1, NULL, &num_devices);
	if (err < 0)
	{
		printf("Couldn't find any devices on selected platform.");
		system("pause");
		exit(1);
	}
	devices = (cl_device_id*)malloc(sizeof(cl_device_id) * num_devices);
	clGetDeviceIDs(platforms[selectedPlatform], CL_DEVICE_TYPE_GPU, num_devices, devices, NULL);
	free(platforms); // free platforms, as it is no longer needed
	displayDeviceInfo(num_devices, devices);

	selectedDevice = 0; // select device 0
	gpu_device = &devices[selectedDevice];

	// Create context
	context = clCreateContext(NULL, num_devices, devices, NULL, NULL, &err);

	// create command queue
	gpu_queue = clCreateCommandQueue(context, *gpu_device, 0, &err);
}


void OpenCLInit::displayPlatformInfo(cl_uint num_platforms, cl_platform_id* platforms)
{
	char* platform_info_data;
	size_t platform_info_size;

	printf("%d platform(s) detected. Platform(s) information:\n\n", num_platforms);

	for (unsigned int i = 0; i < num_platforms; i++)
	{
		// CL_PLATFORM_NAME
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 0, NULL, &platform_info_size);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		platform_info_data = (char*)malloc(platform_info_size);
		if (platform_info_data == NULL)
		{
			printf("Malloc error.\n");
			system("pause");
			exit(1);
		}
		clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, platform_info_size, platform_info_data, NULL);
		printf("Platform %d name: %s\n", i, platform_info_data);
		free(platform_info_data);

		// CL_PLATFORM_VENDOR
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 0, NULL, &platform_info_size);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		platform_info_data = (char*)malloc(platform_info_size);
		if (platform_info_data == NULL)
		{
			printf("Malloc error.\n");
			system("pause");
			exit(1);
		}
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, platform_info_size, platform_info_data, NULL);
		printf("Platform %d vendor: %s\n", i, platform_info_data);
		free(platform_info_data);

		// CL_PLATFORM_VERSION
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 0, NULL, &platform_info_size);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);;
		}
		platform_info_data = (char*)malloc(platform_info_size);
		if (platform_info_data == NULL)
		{
			printf("Malloc error.\n");
			system("pause");
			exit(1);
		}
		clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, platform_info_size, platform_info_data, NULL);
		printf("Platform %d version: %s\n", i, platform_info_data);
		free(platform_info_data);

		// CL_PLATFORM_PROFILE
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, 0, NULL, &platform_info_size);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		platform_info_data = (char*)malloc(platform_info_size);
		if (platform_info_data == NULL)
		{
			printf("Malloc error.\n");
			system("pause");
			exit(1);
		}
		clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, platform_info_size, platform_info_data, NULL);
		printf("Platform %d profile: %s\n", i, platform_info_data);
		free(platform_info_data);

		// CL_PLATFORM_EXTENSIONS
		err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 0, NULL, &platform_info_size);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		platform_info_data = (char*)malloc(platform_info_size);
		if (platform_info_data == NULL)
		{
			printf("Malloc error.\n");
			system("pause");
			exit(1);
		}
		clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, platform_info_size, platform_info_data, NULL);
		printf("Platform %d supports extensions: %s\n", i, platform_info_data);
		free(platform_info_data);
		printf("\n");
	}

}

void OpenCLInit::displayDeviceInfo(cl_uint num_devices, cl_device_id* devices)
{
	char device_string[1024];
	int device_int;

	printf("%d device(s) detected. Device(s) information:\n\n", num_devices);

	for (unsigned int i = 0; i < num_devices; i++)
	{
		// CL_DEVICE_NAME
		err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		printf("Device %d name: %s\n", i, device_string);

		// CL_DEVICE_MAX_COMPUTE_UNITS
		err = clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(device_int), &device_int, NULL);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		printf("Device %d CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", i, device_int);

		// CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
		err = clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(device_int), &device_int, NULL);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		printf("Device %d CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %d\n", i, device_int);
		device_int = 0;

		size_t maxWorkItems[3];
		// CL_DEVICE_MAX_WORK_ITEM_SIZES
		err = clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItems), &maxWorkItems, NULL);
		if (err < 0)
		{
			printf("Couldn't read requested information data.\n");
			system("pause");
			exit(1);
		}
		printf("Device %d CL_DEVICE_MAX_WORK_ITEM_SIZES: %zd x %zd x %zd\n", i, maxWorkItems[0], maxWorkItems[1], maxWorkItems[2]);
		device_int = 0;

		printf("\n");
	}
}