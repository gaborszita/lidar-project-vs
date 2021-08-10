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

// THIS CODE IS NOT USED. ONLY FOR TESTING PURPOSES. UNCOMMENT TO USE.
// WARNING: CODE MAY BE OUT OF DATE.

/*

// Add you host code

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <Windows.h>

#include "PointMover.h"
#include "OpenCLInit.h"

#include "opencl_config.h"
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int main(void) {
	// Create the two input vectors
	int Xold[LIST_SIZE];
	int Yold[LIST_SIZE];
	int Xnew[LIST_SIZE];
	int Ynew[LIST_SIZE];
	for (int i = 0; i < LIST_SIZE; i++) {
		Xold[i] = 0;
		Yold[i] = 0;
		Xnew[i] = 4;
		Ynew[i] = 3;
	}

	OpenCLInit openCLInit;

	PointMover* pointMover = new PointMover(&openCLInit);

	pointMover->enqueuCalculation(Xold, Yold, Xnew, Ynew, 0);

	system("pause");

	return 0;
}

*/