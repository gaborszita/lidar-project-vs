__kernel void calculateOffsets(__global const int *Xold, __global const int *Yold, __global const int *Xnew, __global const int *Ynew, __global float *offsets, __local float* offsetSums, __local int* Xmv, __local int* Ymv, __local float* rotmv, int calcForward, int calcBackward, int calcLeft, int calcRight, int calcLeftRot, int calcRightRot)
{ 
    // Get info of the current work-item and work-group
    const size_t calc = get_group_id(0);
	const size_t local_id = get_local_id(0);
	const size_t local_size = get_local_size(0);
	const size_t group_id = get_group_id(0);

    // Calculate current x, y, rot movement coordinates to be processed
	if(local_id==0) // only compute current coordinates if the work-item's local_id is 0 to avoid multiple unnecessary calculations
	{
		int x  = calcLeft + calcRight;
		int y = calcForward + calcBackward;
		int rot = calcLeftRot + calcRightRot;

		int digits[3] = {y*rot, rot, 1};

		int remainder = calc;
		for (int i=0; i<3; i++)
		{
			int digit = remainder / digits[i];
			remainder = remainder % digits[i];
			digits[i] = digit;
		}

		*Xmv = digits[0] - calcLeft;
		*Ymv = digits[1] - calcBackward;
		*rotmv = (digits[2] - calcLeftRot) * M_PI_F / 180;
	}

	barrier(CLK_LOCAL_MEM_FENCE);

	// copy the individual points to be processed to local memory for faster access
	int xold = Xold[local_id];
	int yold = Yold[local_id];
	int xnew = Xnew[local_id];
	int ynew = Ynew[local_id];
	/*if(group_id==1 && local_id==270)
	{
		int angle = (((int)round(atan2(xold, yold)*180/M_PI_F))+360)%360;
		printf("angle %d, local_id: %d\n", angle, local_id);
	}*/
	if(xnew!=0 && xold!=0)
	{
		// calculate the transformed coordinates
		float xtransformed = (xold+*Xmv)*cos(*rotmv) - (yold+*Ymv)*sin(*rotmv);
		float ytransformed = (xold+*Xmv)*sin(*rotmv) + (yold+*Ymv)*cos(*rotmv);

		int angle = (((int)round(atan2(xtransformed, ytransformed)*180/M_PI_F))+360)%360;
		

		xnew = Xnew[angle];
		ynew = Ynew[angle];

		// calculate offset
		float offset = fabs(xnew - xtransformed) + fabs(ynew - ytransformed);

		// store the calculated offset in the offsetSums local memory, because the individual offsets will be summed up
		offsetSums[local_id] = offset;
	}
	else
	{
		offsetSums[local_id] = 0;
	}

	// Loop for computing offsetSums : divide WorkGroup into 2 parts
	for (uint stride = local_size/2; stride>0; stride /=2)
	{
		// Waiting for each 2x2 addition into given workgroup
		barrier(CLK_LOCAL_MEM_FENCE);

		// Add elements 2 by 2 between local_id and local_id + stride
		if (local_id < stride)
		{
			offsetSums[local_id] += offsetSums[local_id + stride];
		}
	}
	// Write result into offsets[group_id]
	if (local_id == 0) // only copy the offset in global memory if the work-item's local_id is 0 to avoid multiple copies
	{
		offsets[group_id] = offsetSums[0];
	}
}

__kernel void getSmallestOffset(__global const float* offsets, __global int* leastOffsetsIndex, __global float* leastOffsets, __local float* localOffsets, __local int* localOffsetsIndex)
{
	const size_t local_id = get_local_id(0);
	const size_t local_size = get_local_size(0);
	const size_t global_id = get_global_id(0);
	const size_t group_id = get_group_id(0);

	localOffsets[local_id] = offsets[global_id];
	localOffsetsIndex[local_id] = global_id;

	barrier(CLK_LOCAL_MEM_FENCE);
	for (uint stride = local_size/2; stride>1; stride = stride/2 + stride%2)
	{
		barrier(CLK_LOCAL_MEM_FENCE);
		if (local_id < stride)
		{
			if(localOffsets[local_id + stride] < localOffsets[local_id])
			{
				localOffsets[local_id] = localOffsets[local_id + stride];
				localOffsetsIndex[local_id] = localOffsetsIndex[local_id + stride];
			}
		}
	}

	if(localOffsets[1] < localOffsets[0])
	{
		localOffsets[0] = localOffsets[1];
		localOffsetsIndex[0] = localOffsetsIndex[1];
	}

	if (local_id == 0)
	{
		leastOffsetsIndex[group_id] = localOffsetsIndex[0];
		leastOffsets[group_id] = localOffsets[0];
	}
}