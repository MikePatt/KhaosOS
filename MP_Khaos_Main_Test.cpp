/***************************************
*	AUTHOR: Michael Patterson
*	CREATED: 11/15/15
*	PURPOSE: Khaos Operating System: Simulated operating system with a number of PRINTERs, DISKs, and CDROMs.
*	PROF: Eric Schweitzer
*	EDITS: NONE
*****************************************/

#ifndef KHAOS_MAIN_TEST
#define KHAOS_MAIN_TEST
#include<iostream>

#include "MP_Khaos_OpSys.cpp"


int setQuant();
int setMemory();

//Get Quantum and Size of 
const int QUANTUM = setQuant();
const int MEMSIZE = setMemory();

using namespace std;

int main()
{
	FREEMEM = MEMSIZE;
	DevList PRINTERS;
	DevList DISKS;
	DevList CDROMS;
	CPU _CPU;
	SysInit(PRINTERS, DISKS, CDROMS);
	SysRun(PRINTERS, DISKS, CDROMS, _CPU);
}

int setQuant()
{
	int quant;
	std::string input;
	printLine("WELCOME to Khaos OpSys!", '*');
	printLine("Initializing the system", '.');

	while (true)
	{
		std::cout << "How Many Milliseconds are in a Quantum: ";
		getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> quant)
		{
			break;
		}
		std::cout << "\tPlease Enter a Number" << std::endl;
	}
	return quant;
}

int setMemory()
{
	int memsize;
	std::string input;
	while (true)
	{
		std::cout << "What is the Size of Memory in Words: ";
		getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> memsize)
		{
			break;
		}
		std::cout << "\tPlease Enter a Number" << std::endl;
	}
	return memsize;
}
#endif
