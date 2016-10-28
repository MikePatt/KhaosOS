/***************************************
*	AUTHOR: Michael Patterson
*	CREATED: 11/15/15
*	PURPOSE: Khaos Operating System: Simulated operating system with a number of PRINTERs, DISKs, and CDROMs.
*	PROF: Eric Schweitzer
*	EDITS: NONE
*****************************************/
#ifndef KHAOS_OPSYS_CPP
#define KHAOS_OPSYS_CPP

#include "MP_Khaos_Functions.h"
#include <string>
#include <sstream>
#include <limits.h>
#include <stdio.h>


//char locCheck(char, DevList&);

void Commands();
void Snapshot(DevList&, DevList&, DevList&, CPU&, std::deque<PCB*>&, Memory&);
void printLine(std::string, char);

void SysInit(DevList &PRINTERS, DevList &DISKS, DevList &CDROMS)
{
	std::string input;
	int pNum;
	int dNum;
	int cNum;

	// Get the Number of Devices of Each Type
	std::cout << "Enter number of devices:" << std::endl;

	//Initialize Printers
	while (true) 
	{
		std::cout << "\tNumber of PRINTERS [0-9]: ";
		getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> pNum && (pNum >= 0 && pNum <= 9))
			break;
		std::cout << "\tInvalid Number" << std::endl;
 	}
	PRINTERS = DevList(pNum, DevList::_Type::PRINTER);
	//Initialize Disks
	while (true) 
	{
		std::cout << "\tNumber of DISKS [0-9]: ";
		getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> dNum && (dNum >= 0 && dNum <= 9))
			break;
		std::cout << "\tInvalid Number" << std::endl;
 	}
	DISKS = DevList(dNum, DevList::_Type::DISK);

	//Initialize CDROMs
	while (true) 
	{
		std::cout << "\tNumber of CDROMS [0-9]: ";
		getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> cNum && (cNum >= 0 && cNum <= 9))
			break;
		std::cout << "\tInvalid Number" << std::endl;
 	}
	CDROMS = DevList(cNum, DevList::_Type::CDRW);
	printLine("The System is Initialized!", '-');
	return;
}

void SysRun(DevList &PRINTERS, DevList &DISKS, DevList &CDROMS, CPU &_CPU)
{
	int pidCounter = 0;
	std::string input;
	char typeCode[2];
	int locCode = 0;
	Memory MainMemory = Memory(MEMSIZE);
	std::deque<PCB*> Ready_Queue;
	std::deque<PCB*> Job_Pool;
	
	Commands();
	while(true)
	{
		while(true)
		{
			std::cout << "User Command: ";		
	   		getline(std::cin, input);
			if (input.length() == 2) 
			{
	     			typeCode[0] = input[0];
					typeCode[1] = input[1];
					break;
				
	   		}
			else if(input.length() == 1)
			{
				typeCode[0] = input[0];
				if (tolower(typeCode[0]) != 'p' && tolower(typeCode[0]) != 'c' && tolower(typeCode[0]) != 'c')
					break;
				std::cout << "\tP/D/C AND p/d/c must precede a number\t";
			}
			std::cout << "!Invalid Command!\n";
		}

		switch (typeCode[0])
		{
			case 'A': {
						PCB* ins = new PCB(pidCounter);
						if ((ins->segNum * ins->segSize) > MEMSIZE)
						{
							delete ins;
							std::cout << "Process Size Larger Than Memory Size\n";
							break;
						}
						Job_Pool.push_back(ins);
						std::sort(Job_Pool.begin(), Job_Pool.end(), customGreat);
						for (int i = 0; i < Job_Pool.size(); i++)
						{
							
							if((Job_Pool[i]->segNum * Job_Pool[i]->segSize) <= FREEMEM)
							{
								if (!MainMemory.memCheck(Job_Pool[i]))
									MainMemory.allocate(Job_Pool[i]);
								Ready_Queue.push_back(Job_Pool[i]);
								Job_Pool.erase(Job_Pool.begin() + i);
								break;
							}
							else if (i == Job_Pool.size() - 1)
								std::cout << "No Process was Added to Ready Queue!\n";
						}
						break;
					  }
			case 't': MainMemory.free(_CPU.getProcess()); _CPU.endProcess(); break;
			case 'T': _CPU.interrupt(Ready_Queue); break;
			case 'S': Snapshot(PRINTERS, DISKS, CDROMS, _CPU, Ready_Queue, MainMemory); break;
			case 's': Snapshot(PRINTERS, DISKS, CDROMS, _CPU, Ready_Queue, MainMemory); break;
			case 'p': if(PRINTERS.getDevNum() > 0 && !Ready_Queue.empty())
						_CPU.systemCall(typeCode[1] - 48, PRINTERS);
					  else if(PRINTERS.getDevNum() <= 0)
						std::cout << "There Are No PRINTERS!\n";
					  else if(Ready_Queue.empty())
				  		std::cout << "There are No Processes on the READY QUEUE!\n";
					  else
						std::cout << "Unknown PRINTER Error!\n";
					  break;
			case 'P': PRINTERS.interrupt(typeCode[1] - 48, Ready_Queue); break;
			case 'd': if (DISKS.getDevNum() > 0 && !Ready_Queue.empty())
					  {
						  _CPU.systemCall(typeCode[1] - 48, DISKS);
						  DISKS.NHDSA();
					  }
					  else if(DISKS.getDevNum() <= 0)
						std::cout << "There Are No DISKS!\n";
					  else if(Ready_Queue.empty())
				  		std::cout << "There are No Processes on the READY QUEUE!\n";
					  else
						std::cout << "Unknown DISK Error!\n";
					  break;
			case 'D': DISKS.interrupt(typeCode[1] - 48, Ready_Queue); break;
			case 'c': if(CDROMS.getDevNum() > 0 && !Ready_Queue.empty())
				  	_CPU.systemCall(typeCode[1] - 48, CDROMS);
				  else if(CDROMS.getDevNum() <= 0)
					std::cout << "There Are No CDROMS!\n";
				  else if(Ready_Queue.empty())
				  	std::cout << "There are No Processes on the READY QUEUE!\n";
				  else
					std::cout << "Unknown CDROM Error!\n";
				  break;
			case 'C': CDROMS.interrupt(typeCode[1] - 48, Ready_Queue); break;
			case 'h': Commands(); break;
			case 'H': Commands(); break;
			case 'q': std::cout << "\n\tKHAOS OPSYS IS SHUTTING DOWN!" << std::endl; return;
			case 'Q': std::cout << "\n\tKHAOS OPSYS IS SHUTTING DOWN!" << std::endl; return;
			default: std::cout << "\t!Invalid Command!"; break;
		}

		if (_CPU.isEmpty() && !Ready_Queue.empty())
		{
			/*if (!MainMemory.memCheck(Ready_Queue.front()))
				MainMemory.allocate(Ready_Queue.front());*/
			_CPU.grabProcess(Ready_Queue.front());
			Ready_Queue.pop_front();
		}
	}
	return;
}

void Commands()
{
	std::cout << std::endl;
	printLine("Khaos OpSys COMMANDS", '_');
	printLine("", '-');
	std::cout << "Enter any of the following commands:" << std::endl;
	std::cout << "\t A \t: Add a new process" << std::endl;
	std::cout << "\t t \t: Terminate current process" << std::endl;
	std::cout << "\t T \t: Finish a Quantum" << std::endl;
	std::cout << "\t (p/d/c)# : (Lowercase device name )- System Call" << std::endl;
	std::cout << "\t \t  p = Printer; d = Disk; c = CD R/W" << std::endl;
	std::cout << "\t (P/D/C)# : (Uppercase device name ) Interrupts" << std::endl;
	std::cout << "\t \t  P = Printer; D = Disk; C = CD R/W" << std::endl;
	std::cout << "\t s, S \t: Show Snapshot:" << std::endl;
	std::cout << "\t h, H \t: Display this Message." << std::endl;
	std::cout << "\t q, Q \t: Exit" << std::endl << std::endl;
}

void Snapshot(DevList &PRINTERS, DevList &DISKS, DevList &CDROMS, CPU &_CPU, std::deque<PCB*> &Ready_Queue, Memory &MainMemory)
{
	std::string input;
	char entity;
	while (true)
	{
		std::cout << "What Entity Do You Want to See [r/p/d/c/m]:";
		getline(std::cin, input);

		if (input.length() == 1)
		{
			entity = input[0];
			if (tolower(entity) == 'r' || tolower(entity) == 'p' || tolower(entity) == 'd' || tolower(entity) == 'c' || tolower(entity) == 'm')
				break;
			else
				std::cout << "Invalid Input" << std::endl;
		}
	}
	if (tolower(entity) == 'm')
	{
		printf("\n%8s%8s%8s", "PID", "BASE", "LIMIT");
		MainMemory.printMem();
	}
	else if (tolower(entity) != 'd')
		printf("%s%5s%10s%10s%5s%13s%10s%11s%9s%9s", "ENTITY", "PID","FILENAME","MEMSTART","R/W","FILE_LENGTH","CPU_TIME","AVG_BURST", "BASE", "LIMIT");
	else
		printf("%s%5s%10s%10s%5s%13s%10s%11s%9s%9s%9s", "ENTITY", "PID", "FILENAME", "MEMSTART", "R/W", "FILE_LENGTH", "CPU_TIME", "AVG_BURST", "CYL_LOC", "BASE", "LIMIT");
	if (tolower(entity) == 'r')
	{
		printf("%s", "\nRQ: ");
		for (std::deque<PCB*>::iterator it = Ready_Queue.begin(); it != Ready_Queue.end(); ++it)
			(*it)->print();
	}
	else if (tolower(entity) == 'p')
		PRINTERS.print();
	else if (tolower(entity) == 'd')
		DISKS.print();
	else if (tolower(entity) == 'c')
		CDROMS.print();
	_CPU.print();
	return;
}

void printLine(std::string line, char sym)
{
	for (unsigned int i = 0; i < (40 - line.length()); i++)
		std::cout << sym;
	std::cout << line;
	for (unsigned int i = 0; i < (40 - line.length()); i++)
		std::cout << sym;
	std::cout << std::endl;
}
#endif
