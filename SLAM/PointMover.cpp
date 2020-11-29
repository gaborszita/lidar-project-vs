#include "PointMover.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void PointMover::CalcConfig::resetToDeafult()
{
	// default values;
	forwcalc = 5;
	backcalc = 5;
	rightcalc = 5;
	leftcalc = 5;
	rdegcalc = 10;
	ldegcalc = 10;
	NumCalc = forwcalc * backcalc * rightcalc * leftcalc * rdegcalc * ldegcalc;
	NumOfCalcTypes = 7;
}

void PointMover::CalcConfig::setGyroAccordingly(int gyrodeg)
{
	resetToDeafult();
	//rdegcalc += gyrodeg/2;
	//ldegcalc -= gyrodeg/2;
}

PointMover::CalcConfig::CalcConfig(PointMover* pointMover)
{
	this->pointMover = pointMover;
}

PointMover::PointMover(OpenCLInit* openCLInit)
{
	cl_program program;

	FILE* program_handle;
	char* program_buffer;
	size_t program_size;

	this->openCLInit = openCLInit;
	calcConfig = new CalcConfig(this);
	calcConfig->resetToDeafult();

	// set global size and local size of calculateOffsets kernel
	calculateOffsets_global_size = (size_t)calcConfig->NumCalc * LIST_SIZE;
	calculateOffsets_local_size = LIST_SIZE;

	// set global size and local size of getSmallestOffset kernel
	getSmallestOffset_global_size = (size_t)calcConfig->NumCalc;
	size_t maxWorkItems[3];
	// CL_DEVICE_MAX_WORK_ITEM_SIZES
	err = clGetDeviceInfo(*openCLInit->gpu_device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(maxWorkItems), &maxWorkItems, NULL);
	if (err < 0)
	{
		printf("Couldn't read requested information data.\n");
		system("pause");
		exit(1);
	}
	for (getSmallestOffset_local_size = maxWorkItems[0]; getSmallestOffset_global_size % getSmallestOffset_local_size != 0; getSmallestOffset_local_size--);

	leastOffsets = (float*)malloc(getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(float));
	if (leastOffsets == NULL)
	{
		printf("Malloc error.\n");
		system("pause");
		exit(1);
	}
	leastOffsetsIndex = (int*)malloc(getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(int));
	if (leastOffsetsIndex == NULL)
	{
		printf("Malloc error.\n");
		system("pause");
		exit(1);
	}

	// load the kernel
	int err = fopen_s(&program_handle, "kernel_code.cl", "rb");
	if (err) {
		fprintf(stderr, "Failed to load kernel file.\n");
		system("pause");
		exit(1);
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	program_buffer = (char*)malloc(program_size + 1);
	if (program_buffer == NULL)
	{
		printf("Malloc error.\n");
		system("pause");
		exit(1);
	}
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);

	// Create a program from the kernel source
	program = clCreateProgramWithSource(openCLInit->context, 1, (const char**)&program_buffer, &program_size, &err);
	free(program_buffer);

	char options[] = "-cl-fast-relaxed-math";
	// Build the program
	err = clBuildProgram(program, 1, openCLInit->gpu_device, /*options*/NULL, NULL, NULL);
	if (err)
	{
		printf("Program build errors/warnings detected. Return code: %d\n", err);
		size_t log_size;
		clGetProgramBuildInfo(program, *openCLInit->gpu_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char* program_log = (char*)calloc(log_size + 1, sizeof(char));
		clGetProgramBuildInfo(program, *openCLInit->gpu_device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		system("pause");
		exit(1);
	}
	else
	{
		printf("Program build succeeded. Return code: %d\n", err);
		size_t log_size;
		clGetProgramBuildInfo(program, *openCLInit->gpu_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char* program_log = (char*)calloc(log_size + 1, sizeof(char));
		clGetProgramBuildInfo(program, *openCLInit->gpu_device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
	}

	// Create the OpenCL kernel
	calculateOffsets_kernel = clCreateKernel(program, "calculateOffsets", &err);
	if (err)
	{
		printf("Error creating calculateOffsets_kernel kernel\n");
		system("pause");
		exit(1);
	}
	getSmallestOffset_kernel = clCreateKernel(program, "getSmallestOffset", &err);
	if (err)
	{
		printf("Error creating getSmallestOffset_kernel kernel\n");
		system("pause");
		exit(1);
	}

	/*size_t wg_size = 0;
	clGetKernelWorkGroupInfo(kernel, devices[selectedDevice], CL_KERNEL_WORK_GROUP_SIZE, sizeof(wg_size), &wg_size, NULL);
	printf("CL_KERNEL_WORK_GROUP_SIZE: %zd\n", wg_size);*/

	// Create memory buffers on the device for each vector 
	Xold_mem_obj = clCreateBuffer(openCLInit->context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
	Yold_mem_obj = clCreateBuffer(openCLInit->context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
	Xnew_mem_obj = clCreateBuffer(openCLInit->context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
	Ynew_mem_obj = clCreateBuffer(openCLInit->context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
	offsets_mem_obj = clCreateBuffer(openCLInit->context, CL_MEM_READ_WRITE, calcConfig->NumCalc * sizeof(float), NULL, &err);

	leastOffsetsIndex_mem_obj = clCreateBuffer(openCLInit->context, CL_MEM_WRITE_ONLY, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(int), NULL, &err);
	leastOffsets_mem_obj = clCreateBuffer(openCLInit->context, CL_MEM_WRITE_ONLY, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(float), NULL, &err);

	// Set the arguments of the calculateOffsets_kernel
	clSetKernelArg(calculateOffsets_kernel, 0, sizeof(cl_mem), (void*)&Xold_mem_obj);
	clSetKernelArg(calculateOffsets_kernel, 1, sizeof(cl_mem), (void*)&Yold_mem_obj);
	clSetKernelArg(calculateOffsets_kernel, 2, sizeof(cl_mem), (void*)&Xnew_mem_obj);
	clSetKernelArg(calculateOffsets_kernel, 3, sizeof(cl_mem), (void*)&Ynew_mem_obj);
	clSetKernelArg(calculateOffsets_kernel, 4, sizeof(cl_mem), (void*)&offsets_mem_obj);
	clSetKernelArg(calculateOffsets_kernel, 5, calculateOffsets_local_size * sizeof(float), NULL);
	clSetKernelArg(calculateOffsets_kernel, 6, sizeof(int), NULL);
	clSetKernelArg(calculateOffsets_kernel, 7, sizeof(int), NULL);
	clSetKernelArg(calculateOffsets_kernel, 8, sizeof(float), NULL);
	setKernelCalcConfigArgs();

	// Set the arguments of the getSmallestOffset_kernel
	clSetKernelArg(getSmallestOffset_kernel, 0, sizeof(cl_mem), (void*)&offsets_mem_obj);
	clSetKernelArg(getSmallestOffset_kernel, 1, sizeof(cl_mem), (void*)&leastOffsetsIndex_mem_obj);
	clSetKernelArg(getSmallestOffset_kernel, 2, sizeof(cl_mem), (void*)&leastOffsets_mem_obj);
	clSetKernelArg(getSmallestOffset_kernel, 3, getSmallestOffset_local_size * sizeof(float), NULL);
	clSetKernelArg(getSmallestOffset_kernel, 4, getSmallestOffset_local_size * sizeof(int), NULL);

	Xold_mem_obj_ptr = clEnqueueMapBuffer(openCLInit->gpu_queue, Xold_mem_obj, CL_TRUE, CL_MAP_WRITE, 0, LIST_SIZE * sizeof(int), 0, NULL, NULL, &err);
	Yold_mem_obj_ptr = clEnqueueMapBuffer(openCLInit->gpu_queue, Yold_mem_obj, CL_TRUE, CL_MAP_WRITE, 0, LIST_SIZE * sizeof(int), 0, NULL, NULL, &err);
	Xnew_mem_obj_ptr = clEnqueueMapBuffer(openCLInit->gpu_queue, Xnew_mem_obj, CL_TRUE, CL_MAP_WRITE, 0, LIST_SIZE * sizeof(int), 0, NULL, NULL, &err);
	Ynew_mem_obj_ptr = clEnqueueMapBuffer(openCLInit->gpu_queue, Ynew_mem_obj, CL_TRUE, CL_MAP_WRITE, 0, LIST_SIZE * sizeof(int), 0, NULL, NULL, &err);
	leastOffsetsIndex_mem_obj_ptr = clEnqueueMapBuffer(openCLInit->gpu_queue, leastOffsetsIndex_mem_obj, CL_TRUE, CL_MAP_READ, 0, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(int), 0, NULL, NULL, &err);
	leastOffsets_mem_obj_ptr = clEnqueueMapBuffer(openCLInit->gpu_queue, leastOffsets_mem_obj, CL_TRUE, CL_MAP_READ, 0, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(int), 0, NULL, NULL, &err);

	// Clean up
	//clReleaseContext(openCLInit->context);
	clReleaseProgram(program);
}

PointMover::~PointMover()
{
	clEnqueueUnmapMemObject(openCLInit->gpu_queue, Xold_mem_obj, Xold_mem_obj_ptr, 0, NULL, NULL);
	clEnqueueUnmapMemObject(openCLInit->gpu_queue, Yold_mem_obj, Yold_mem_obj_ptr, 0, NULL, NULL);
	clEnqueueUnmapMemObject(openCLInit->gpu_queue, Xnew_mem_obj, Xnew_mem_obj_ptr, 0, NULL, NULL);
	clEnqueueUnmapMemObject(openCLInit->gpu_queue, Ynew_mem_obj, Ynew_mem_obj_ptr, 0, NULL, NULL);
	clEnqueueUnmapMemObject(openCLInit->gpu_queue, leastOffsetsIndex_mem_obj, leastOffsetsIndex_mem_obj_ptr, 0, NULL, NULL);
	clEnqueueUnmapMemObject(openCLInit->gpu_queue, leastOffsets_mem_obj, leastOffsets_mem_obj_ptr, 0, NULL, NULL);
	clReleaseKernel(calculateOffsets_kernel);
	clReleaseKernel(getSmallestOffset_kernel);
	clReleaseMemObject(Xold_mem_obj);
	clReleaseMemObject(Yold_mem_obj);
	clReleaseMemObject(Xnew_mem_obj);
	clReleaseMemObject(Ynew_mem_obj);
	clReleaseMemObject(offsets_mem_obj);
	clReleaseMemObject(leastOffsetsIndex_mem_obj);
	clReleaseMemObject(leastOffsets_mem_obj);
	free(leastOffsets);
	free(leastOffsetsIndex);
}

void PointMover::setKernelCalcConfigArgs()
{
	clSetKernelArg(calculateOffsets_kernel, 9, sizeof(int), &calcConfig->forwcalc);
	clSetKernelArg(calculateOffsets_kernel, 10, sizeof(int), &calcConfig->backcalc);
	clSetKernelArg(calculateOffsets_kernel, 11, sizeof(int), &calcConfig->leftcalc);
	clSetKernelArg(calculateOffsets_kernel, 12, sizeof(int), &calcConfig->rightcalc);
	clSetKernelArg(calculateOffsets_kernel, 13, sizeof(int), &calcConfig->ldegcalc);
	clSetKernelArg(calculateOffsets_kernel, 14, sizeof(int), &calcConfig->rdegcalc);
}

int PointMover::enqueuCalculation(int Xold[LIST_SIZE], int Yold[LIST_SIZE], int Xnew[LIST_SIZE], int Ynew[LIST_SIZE], int gyrodeg)
{
	calcConfig->resetToDeafult();
	calcConfig->setGyroAccordingly(gyrodeg);

	// Copy the lists and variables to their respective memory buffers
	/*clEnqueueWriteBuffer(openCLInit->gpu_queue, Xold_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), Xold, 0, NULL, NULL);
	clEnqueueWriteBuffer(openCLInit->gpu_queue, Yold_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), Yold, 0, NULL, NULL);
	clEnqueueWriteBuffer(openCLInit->gpu_queue, Xnew_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), Xnew, 0, NULL, NULL);
	clEnqueueWriteBuffer(openCLInit->gpu_queue, Ynew_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), Ynew, 0, NULL, NULL);*/
	memcpy(Xold_mem_obj_ptr, Xold, LIST_SIZE * sizeof(int));
	memcpy(Yold_mem_obj_ptr, Yold, LIST_SIZE * sizeof(int));
	memcpy(Xnew_mem_obj_ptr, Xnew, LIST_SIZE * sizeof(int));
	memcpy(Ynew_mem_obj_ptr, Ynew, LIST_SIZE * sizeof(int));


	err = clEnqueueNDRangeKernel(openCLInit->gpu_queue, calculateOffsets_kernel, 1, NULL, &calculateOffsets_global_size, &calculateOffsets_local_size, 0, NULL, NULL);
	err = clEnqueueNDRangeKernel(openCLInit->gpu_queue, getSmallestOffset_kernel, 1, NULL, &getSmallestOffset_global_size, &getSmallestOffset_local_size, 0, NULL, NULL);
	isProcessing = true;

	return 0;
}

int PointMover::getCalculationData(int *Xmv, int *Ymv, int *rotmv)
{
	if (!isProcessing)
	{
		return NO_PROCESSING_QUEUED;
	}
	clFinish(openCLInit->gpu_queue);

	/*clEnqueueReadBuffer(openCLInit->gpu_queue, leastOffsets_mem_obj, CL_TRUE, 0, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(float), leastOffsets, 0, NULL, NULL);
	clEnqueueReadBuffer(openCLInit->gpu_queue, leastOffsetsIndex_mem_obj, CL_TRUE, 0, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(int), leastOffsetsIndex, 0, NULL, NULL);*/
	memcpy(leastOffsets, leastOffsets_mem_obj_ptr, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(float));
	memcpy(leastOffsetsIndex, leastOffsetsIndex_mem_obj_ptr, getSmallestOffset_global_size / getSmallestOffset_local_size * sizeof(int));

	float smallestValue = leastOffsets[0];
	int smallestValueIndex = 0;
	for (int i = 1; i < getSmallestOffset_global_size / getSmallestOffset_local_size; i++)
	{
		if (leastOffsets[i] < smallestValue)
		{
			smallestValue = leastOffsets[i];
			smallestValueIndex = leastOffsetsIndex[i];
		}
	}

	int x = calcConfig->leftcalc + calcConfig->rightcalc;
	int y = calcConfig->forwcalc + calcConfig->backcalc;
	int rot = calcConfig->ldegcalc + calcConfig->rdegcalc;

	int digits[3] = { y*rot, rot, 1 };

	int remainder = smallestValueIndex;
	for (int i = 0; i < 3; i++)
	{
		int digit = remainder / digits[i];
		remainder = remainder % digits[i];
		digits[i] = digit;
	}

	*Xmv = digits[0] - calcConfig->leftcalc;
	*Ymv = digits[1] - calcConfig->backcalc;
	*rotmv = digits[2] - calcConfig->ldegcalc;

	//printf("x:%d y:%d deg:%d\n\n", Xmv, Ymv, rotmv);
	isProcessing = false;

	return 0;
}