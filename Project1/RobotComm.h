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

#ifndef _ROBOTCOMM_H
#define _ROBOTCOMM_H


#include "MyForm.h"
#include <vcclr.h>
namespace Project1
{
	class RobotComm
	{
	private:
		gcroot<MyForm^> myForm;
		class MotorSpeeds
		{
		public:
			int m1;
			int m2;
			int m3;
			int m4;
			static void setMotorSpeeds(MotorSpeeds* ms);
		};

	public:
		RobotComm(MyForm^ myForm);
		void maintcp();
	};
}

#endif