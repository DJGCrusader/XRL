#include <iostream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <unistd.h>
#include <termios.h>
#include <cmath>
#include <queue>

#include "../include/RoboteqDevice.h"
#include "../include/ErrorCodes.h"
#include "../include/Constants.h"

#define NB_ENABLE 0
#define NB_DISABLE 1
#define _ND 2 //Number of devices
#define CTRLFREQ 50
#define CTRLPERIOD (1.0/CTRLFREQ)
#define LATEPERIOD (CTRLPERIOD/2.0)
#define PI 3.1415926
#define ENC2RAD (2*PI/90.0)
#define ENC2CMD 1000.0/180.0

using namespace std;
	
//Helper Function to read user input while still running loop, for quitting
void nonblock(int state)
{
    struct termios ttystate;
 
    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
 
    if (state==NB_ENABLE)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==NB_DISABLE)
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
 
}

// //Check user input while still running loop, for quitting
int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

int main(int argc, char *argv[])
{
	////////////////////////////////////////////////////////////	Initialize All
	char inChar;
    int inCheck=0;
    nonblock(NB_ENABLE);

	RoboteqDevice devices[_ND];	
	int status;
	for(int i=0; i<_ND; i++){
		status = devices[i].Connect("/dev/ttyACM"+std::to_string(i));
		if(status != RQ_SUCCESS){
			cout<<"Error connecting to device: "<<status<<"."<<endl;
			return 1;
		}
	}

	signed long int cmd[2*_ND];  //Can be +-1000, mapping to +-180 hall counts, +-4pi
	for(int i=0; i<_ND; i++){
		cmd[i] = 0;
		cmd[i+1] = 0;
		devices[i].SetCommand(_M, cmd[i], cmd[i+1]);
	}

	//	Trajectory
    double movement_duration = 3;      //duration in seconds

	//  Record start time
    std::chrono::time_point<std::chrono::high_resolution_clock>
                        totalStart = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock>
                        trajStart = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock>
                        startT = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock>
                        finishT = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finishT - startT;    
    std::chrono::duration<double> elapsed2 = finishT - startT;    
    std::chrono::duration<double> totalElapsed = finishT - totalStart;
	int isRunning = true;

    double over = 0;
    double tt = 0;

	//////////////////////////////////////////////////////////////////	Run
	while(isRunning){
		//  Timing to kep a certain frequency
        finishT = std::chrono::high_resolution_clock::now();
        elapsed = finishT - startT;
        totalElapsed = finishT - totalStart;
        elapsed2 = finishT - trajStart;
        if(elapsed.count() >= CTRLPERIOD){
            over = elapsed.count()-(CTRLPERIOD);
        	tt = elapsed2.count();
            startT = std::chrono::high_resolution_clock::now();

            if( over > LATEPERIOD){
                cout<<"SLOW! Loop took "<< over << " seconds over" << endl;
            }

            if(tt < movement_duration){
            	cmd[0] = 0; //*(tt/movement_duration)*ENC2CMD*(PI/2.0)/ENC2RAD;
            	cmd[1] = 0; //-(tt/movement_duration)*ENC2CMD*(PI/2.0)/ENC2RAD;
            	cmd[2] = -(tt/movement_duration)*ENC2CMD*(PI/1.0)/ENC2RAD;
            	cmd[3] = (tt/movement_duration)*ENC2CMD*(PI/1.0)/ENC2RAD;
            }else if(tt < 2*movement_duration){
            	cmd[0] = 0;//(( 2*movement_duration-tt)/movement_duration)*ENC2CMD*(PI/2.0)/ENC2RAD;
            	cmd[1] = 0;//-(( 2*movement_duration-tt)/movement_duration)*ENC2CMD*(PI/2.0)/ENC2RAD;
            	cmd[2] = -(( 2*movement_duration-tt)/movement_duration)*ENC2CMD*(PI/1)/ENC2RAD;
            	cmd[3] = (( 2*movement_duration-tt)/movement_duration)*ENC2CMD*(PI/1)/ENC2RAD;
            }else{
            	trajStart = std::chrono::high_resolution_clock::now();
                // cout<<"\nQuitting!"<< endl;
                // isRunning=0;
                // inCheck = 1;
                // nonblock(NB_DISABLE);            	
            }

        	for(int i=0; i<_ND; i++){
				devices[i].SetCommand(_M, cmd[2*i], cmd[2*i+1]);
			}

	        //Quit when user presses 'q'
	        inCheck=kbhit();
	        if (inCheck!=0)
	        {
	            inChar=fgetc(stdin);
	            if (inChar=='q'){
	                cout<<"\nQuitting!"<< endl;
	                isRunning=0;
	                inCheck = 1;
	                nonblock(NB_DISABLE);
	            }else{
	                isRunning=1;
	                inCheck = 0;
	            }
	        }
        }
	}

	//////////////////////////////////////////////////////////////	Close All
		for(int i=0; i<_ND; i++){
		devices[i].SetCommand(_M, 0, 0);
	}

	for(int i=0; i<_ND; i++){
		devices[0].Disconnect();
	}
	return 0;
}