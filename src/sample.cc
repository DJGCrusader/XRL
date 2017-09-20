#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../include/RoboteqDevice.h"
#include "../include/ErrorCodes.h"
#include "../include/Constants.h"

using namespace std;

int main(int argc, char *argv[])
{
	string response = "";
	RoboteqDevice device;
	int status = device.Connect("/dev/ttyACM1");

	if(status != RQ_SUCCESS)
	{
		cout<<"Error connecting to device: "<<status<<"."<<endl;
		return 1;
	}

	// cout<<"- SetConfig(_MMOD, 1, 4)...";
	// if((status = device.SetConfig(_MMOD, 1, 4)) != RQ_SUCCESS)
	// 	cout<<"failed --> "<<status<<endl;
	// else
	// 	cout<<"succeeded."<<endl;

	//Wait 10 ms before sending another command to device
	sleepms(10);

	// int result;
	// cout<<"- GetConfig(_MMOD, 1)...";
	// if((status = device.GetConfig(_MMOD, 1, result)) != RQ_SUCCESS)
	// 	cout<<"failed --> "<<status<<endl;
	// else
	// 	cout<<"returned --> "<<result<<endl;

	// //Wait 10 ms before sending another command to device
	// sleepms(10);

	// cout<<"- GetValue(_ANAIN, 1)...";
	// if((status = device.GetValue(_ANAIN, 1, result)) != RQ_SUCCESS)
	// 	cout<<"failed --> "<<status<<endl;
	// else
	// 	cout<<"returned --> "<<result<<endl;

	// //Wait 10 ms before sending another command to device
	// sleepms(10);

	signed long int cmd= 100; //-80*2147483648/100;

	if((status = device.SetCommand(_M, -100, 100) != RQ_SUCCESS))
		cout<<"failed --> "<<status<<endl;
	else
		cout<<"succeeded."<<endl;

	//Wait 1s before sending another command to device
	usleep(1000000);

	if((status = device.SetCommand(_M, 0, 0)) != RQ_SUCCESS)
		cout<<"failed --> "<<status<<endl;
	else
		cout<<"succeeded."<<endl;

	device.Disconnect();
	return 0;
}
