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

#ifndef _POINTMOVER_H
#define _POINTMOVER_H

#include "OpenCLInit.h"
#define LIST_SIZE 360

#include "opencl_config.h"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define NO_PROCESSING_QUEUED -2

class PointMover
{
private:
	cl_kernel calculateOffsets_kernel;
	cl_kernel getSmallestOffset_kernel;
	cl_int err;
	cl_mem Xold_mem_obj;
	cl_mem Yold_mem_obj;
	cl_mem Ynew_mem_obj;
	cl_mem Xnew_mem_obj;
	cl_mem offsets_mem_obj;
	cl_mem leastOffsetsIndex_mem_obj;
	cl_mem leastOffsets_mem_obj;
	size_t calculateOffsets_global_size;
	size_t calculateOffsets_local_size;
	size_t getSmallestOffset_global_size;
	size_t getSmallestOffset_local_size;
	OpenCLInit* openCLInit;
	float* leastOffsets;
	int* leastOffsetsIndex;
	bool isProcessing = false;

	void* Xold_mem_obj_ptr;
	void* Yold_mem_obj_ptr;
	void* Xnew_mem_obj_ptr;
	void* Ynew_mem_obj_ptr;
	void* leastOffsets_mem_obj_ptr;
	void* leastOffsetsIndex_mem_obj_ptr;

	class CalcConfig
	{
	private:
		PointMover* pointMover;
	public:
		int forwcalc;
		int backcalc;
		int rightcalc;
		int leftcalc;
		int rdegcalc;
		int ldegcalc;
		int NumCalc;
		int NumOfCalcTypes;

		void resetToDeafult();
		void setGyroAccordingly(int gyrodeg);
		CalcConfig(PointMover* pointMover);
	};
	CalcConfig* calcConfig;

	void setKernelCalcConfigArgs();

public:
	PointMover(OpenCLInit* openCLInit);
	~PointMover();
	int enqueuCalculation(int Xold[LIST_SIZE], int Yold[LIST_SIZE], int Xnew[LIST_SIZE], int Ynew[LIST_SIZE], int gyrodeg);
	int getCalculationData(int *x, int *y, int *rotmv);
};

#endif