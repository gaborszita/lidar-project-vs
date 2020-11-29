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