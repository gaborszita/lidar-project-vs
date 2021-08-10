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

#ifndef _BITARRAY2D_H
#define _BITARRAY2D_H

namespace BitArrays
{
	class BitArray2D 
	{
	private:
		long int* arr;
		size_t spY;
		size_t totalArraySize;

	public:
		BitArray2D(size_t dimX, size_t dimY);
		~BitArray2D();
		short int getbit(size_t bitreqX, size_t bitreqY);
		short int setbit(size_t bitreqX, size_t bitreqY, short int bitwrite);
	};
}

#endif