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