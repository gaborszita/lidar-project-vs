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

#include "BitArrays.h"
#include <stdlib.h>
#include <string.h>

namespace BitArrays
{
	BitArray2D::BitArray2D(size_t dimX, size_t dimY) 
	{
		arr = (long int*)malloc(((dimX - 1) * dimY + dimY + 32 - 1) / 32 * sizeof(long int));
		if (arr == 0)
		{
			exit(BITARRAY_NO_MEM_AVAILABLE);
		}
		memset(arr, 0, ((dimX-1) * dimY + dimY + 32 - 1) / 32 * sizeof(long int));
		spY = dimY;
		totalArraySize = ((dimX - 1) * dimY + dimY + 32 - 1) / 32;
	}
	BitArray2D::~BitArray2D()
	{
		free(arr);
	}
	short int BitArray2D::getbit(size_t bitreqX, size_t bitreqY) 
	{
		size_t bitreq = bitreqX * spY + bitreqY;
		size_t index = bitreq / 32;
		size_t bit = bitreq % 32;
		//printf("bitreq: %d index:%d bit:%d\n", bitreq, index, bit);
		//return 0;
		if (index < totalArraySize)
		{
			short int elem = (arr[index] >> bit) & 1;
			return elem;
		}
		else
		{
			return -1;
		}
	}
	short int BitArray2D::setbit(size_t bitreqX, size_t bitreqY, short int bitwrite) 
	{
		size_t bitreq = bitreqX * spY + bitreqY;
		size_t index = bitreq / 32;
		size_t bit = bitreq % 32;
		//printf("index:%d bit7:%d\n", index, bit);
		//return;
		if (index < totalArraySize)
		{
			if (bitwrite == 0) 
			{
				arr[index] &= ~(1 << bit);
			}
			else 
			{
				arr[index] |= (1 << bit);
			}
			return 0;
		}
		else
		{
			return -1;
		}
	}
}