/***************************************
*	AUTHOR: Michael Patterson
*	CREATED: 11/15/15
*	PURPOSE: Khaos Operating System: Simulated operating system with a number of PRINTERs, DISKs, and CDROMs.
*	PROF: Eric Schweitzer
*	EDITS: NONE
*****************************************/
#ifndef KHAOS_FUNCTIONS_H
#define KHAOS_FUNCTIONS_H

#include "MP_Khaos_Structs_Classes.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <sstream>
#include <cmath>

/*****************Memory Functions******************/
//MEMORY CONSTRUCTOR
Memory::Memory(int size) : physAdd(size, std::make_pair(-1, 0))
{
	holes.push_back(std::make_pair(0, size - 1));
}

void Memory::allocate(PCB* add)
{
	if (add->pid != -1)
	{
		for (int i = 0; i < holes.size(); i++)
		{
			if ((holes[i].second - holes[i].first) >= (add->segNum * add->segSize))
			{
				//int mult1 = log2(add->segSize) + 1;
				//int mult2 = log2(add->segNum) + 1;

				for (int j = 0; j < (add->segNum * add->segSize); j++)
				{
						physAdd[holes[i].first + j] = std::make_pair(add->pid, j);
				}
				if (holes[i].second == holes[i].first + (add->segNum * add->segSize) - 1)
					holes.erase(holes.begin() + i);
				else
					holes[i].first += (add->segNum * add->segSize);
				bool here = false;
				for (int j = 0; j < inMem.size(); j++)
				{
					if (inMem[j]->pid == add->pid)
						here = true;
				}
				if (!here)
					inMem.push_back(add);
				FREEMEM -= (add->segNum * add->segSize);
				break;
			}
			else if ((add->segNum * add->segSize) <= FREEMEM)
			{
				this->compact();
				for (int j = 0; j < (add->segNum * add->segSize); j++)
				{
						physAdd[holes[i].first + j] = std::make_pair(add->pid, j);
				}
				if (holes[i].second == holes[i].first + (add->segNum * add->segSize) - 1)
					holes.erase(holes.begin() + i);
				else
					holes[i].first += (add->segNum * add->segSize);
				bool here = false;
				for (int j = 0; j < inMem.size(); j++)
				{
					if (inMem[j]->pid == add->pid)
						here = true;
				}
				if(!here)
					inMem.push_back(add);
				FREEMEM -= (add->segNum * add->segSize);
				break;
			}
		}
	}
	return;
}

void Memory::free(PCB* del)
{
	int spot;
	for (int i = 0; i < physAdd.size(); i++)
	{
		if (physAdd[i].first == del->pid)
		{
			physAdd[i] = std::make_pair(-1, 0);
			spot = i;
		}
	}
	holes.push_back(std::make_pair(spot - ((del->segNum * del->segSize )- 1)  , spot));
	for (int j = 0; j < holes.size() - 1; j++)
	{
		for (int k = j + 1; k < holes.size(); k++)
		{
			if (holes[j].second == holes[k].first - 1 || holes[j].second == holes[k].first)
			{
				holes[j].second = holes[k].second;
				holes.erase(holes.begin() + k);
				//k--;
			}
		}
	}
	for (int j = 0; j < inMem.size(); j++)
	{
		if (inMem[j] == del)
		{
			inMem.erase(inMem.begin() + j);
			FREEMEM += (del->segNum * del->segSize);
			return;
		}
	}
	std::cout << "Process Not Found in Memory!\n";
	return;
}

void Memory::compact()
{
	for (int i = 0; i < physAdd.size(); i++)
	{
		physAdd[i] = std::make_pair(-1, 0);
	}
	FREEMEM = MEMSIZE;
	holes.clear();
	holes.push_back(std::make_pair(0, MEMSIZE - 1));
	for (int i = 0; i < inMem.size(); i++)
	{
		allocate(inMem[i]);
	}
}

void Memory::printMem()
{
	std::cout << "\nMEMORY:";
	for (int i = 0; i < physAdd.size(); i++)
	{
		if (inMem.size() == 0)
		{
			printf("\n%s%8s%8d\t%d", "ENTITY", "XXXX", holes[0].first, holes[0].second + 1);
			break;
		}
		else
		{
			for (int j = 0; j < inMem.size(); j++)
			{
				if (physAdd[i].first == inMem[j]->pid)
				{
					printf("\n%8d%8d\t%d", physAdd[i].first, i, i + inMem[j]->segSize);
					i += (inMem[j]->segSize - 1);
					break;
				}
				else if (physAdd[i].first == (-1))
				{
					for (int k = 0; k < holes.size(); k++)
					{
						if (i == holes[k].first)
						{
							printf("\n%8s%8d\t%d", "XXXX", holes[k].first, holes[k].second);
							i += holes[k].second;
							break;
						}
					}
				}
			}
		}
	}
}

//void Memory::printHoles()
//{
//	printf("\n%8s\t%8s", "BEGIN", "END");
//	for (int i = 0; i < holes.size(); i++)
//	{
//		printf("\n%8d\t%8d", holes[i].first, holes[i].second);
//	}
//}
//
//void Memory::printInMem()
//{
//	printf("\n%s%5s%10s%10s%5s%13s%10s%11s", "ENTITY", "PID", "FILENAME", "MEMSTART", "R/W", "FILE_LENGTH", "CPU_TIME", "AVG_BURST");
//	for (int i = 0; i < inMem.size(); i++)
//	{
//		inMem[i]->print();
//	}
//}

bool Memory::memCheck(PCB* check)
{
	for (int i = 0; i < inMem.size(); i++)
	{
		if (inMem[i] == check)
			return true;
	}
	return false;
}
/*****************PCB Functions******************/
PCB::PCB()
{

}

PCB::PCB(int& id): pid(id)
{
	id++;
	int num;
	int size;
	std::string input;
	while (true)
	{
		std::cout << "How Many Segments does this Process Have: ";
		getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> num)
			break;
		std::cout << "\tInvalid Number" << std::endl;
	}
	segNum = num;

	while (true)
	{
		std::cout << "What is the Size of each Segment in Words: ";
		getline(std::cin, input);

		// This code converts from string to number safely.
		std::stringstream myStream(input);
		if (myStream >> size)
			break;
		std::cout << "\tInvalid Number" << std::endl;
	}
	segSize = size;
}

void PCB::print()
{
	printf("\n%10d%10s%10d%5c%13d%10d%10.2f%9d%9d", pid, fileName.c_str(), startLoc, action, fileLen, CPUTime, AvgBurstTime, startLoc, segNum * segSize);
	return;
}

void PCB::printDisk()
{
	printf("\n%10d%10s%10d%5c%13d%10d%10.2f%10d%9d%9d", pid, fileName.c_str(), startLoc, action, fileLen, CPUTime, AvgBurstTime,cyLoc, startLoc, segNum * segSize);
	return;
}

/*****************Device Functions******************/
void DevList::Device::swap(PCB* A, PCB* B)
{
	PCB temp;

	temp.pid = A->pid;
	temp.fileName = A->fileName;
	temp.startLoc = A->startLoc;
	temp.action = A->action;
	temp.fileLen = A->fileLen;
	temp.next = A->next;
	temp.CPUTime = A->CPUTime;
	temp.CPUCalls = A->CPUCalls;
	temp.AvgBurstTime = A->AvgBurstTime;
	temp.cyLoc = A->cyLoc;

	A->pid = B->pid;
	A->fileName = B->fileName;
	A->startLoc = B->startLoc;
	A->action = B->action;
	A->fileLen = B->fileLen;
	A->next = B->next;
	A->CPUTime = B->CPUTime;
	A->CPUCalls = B->CPUCalls;
	A->AvgBurstTime = B->AvgBurstTime;
	A->cyLoc = B->cyLoc;

	B->pid = temp.pid;
	B->fileName = temp.fileName;
	B->startLoc = temp.startLoc;
	B->action = temp.action;
	B->fileLen = temp.fileLen;
	B->next = temp.next;
	B->CPUTime = temp.CPUTime;
	B->CPUCalls = temp.CPUCalls;
	B->AvgBurstTime = temp.AvgBurstTime;
	B->cyLoc = temp.cyLoc;
	return;
}

void DevList::Device::print(_Type devType)
{
	if (devType == DevList::_Type::DISK)
	{
		for (std::deque<PCB*>::iterator it = devQ.begin(); it != devQ.end(); ++it)
			(*it)->printDisk();
	}
	else
	{
		for (std::deque<PCB*>::iterator it = devQ.begin(); it != devQ.end(); ++it)
			(*it)->print();
	}
	return;
}

/*****************DevList Functions******************/
DevList::DevList()
{

}

DevList::DevList(int amt, _Type type) : devType(type), Devs(amt)
{
	if (devType == DevList::_Type::DISK)
	{
		int cylinders;
		std::string input;
		for (int i = 0; i < amt; i++)
		{
			while (true)
			{
				std::cout << "\t\tHow Many Cylinders Does Disk " << i + 1 << " Have: ";
				getline(std::cin, input);

				// This code converts from string to number safely.
				std::stringstream myStream(input);
				if (myStream >> cylinders)
					break;
				std::cout << "\tInvalid Number" << std::endl;
			}
			Devs[i].numCyl = cylinders;
		}
	}
}

void DevList::interrupt(int devLoc, std::deque<PCB*>& Ready)
{
	Devs[devLoc - 1].headPos = Devs[devLoc - 1].devQ.front()->cyLoc;
	Ready.push_back(Devs[devLoc - 1].devQ.front());
	Devs[devLoc - 1].devQ.pop_front();
	return;	
}

void DevList::print()
{
	int count = 1;
	for (std::vector<Device>::iterator it = Devs.begin(); it != Devs.end(); ++it)
	{
		//std::cout << "There are " << Devs.size() << "Devices here!\n";
		switch(devType)
		{
			case PRINTER: std::cout <<"\n p" << count << ":"; break;
			case DISK: std::cout << "\n d" << count << ":"; break;
			case CDRW: std::cout << "\n c" << count << ":"; break;
		}
		count++;
		it->print(devType);
	}
	return;
}

void DevList::NHDSA()
{
	if(devType == DevList::_Type::DISK)
	{
		for (unsigned int i = 0; i < Devs.size(); i++)
		{
			int size = Devs[i].devQ.size();
			if (size > 0)
			{
				std::sort(Devs[i].devQ.begin(), Devs[i].devQ.end(), customLess);
				int sub = 0;
				int position = 0;
				for (unsigned int q = 0; q < Devs[i].devQ.size(); q++)
				{
					//std::cout << "q: " << q << "| Head Position: " << Devs[i].headPos << "| Cyloc: " << Devs[i].devQ[q]-> cyLoc << std::endl;
					if (Devs[i].devQ[q]->cyLoc >= Devs[i].headPos)
					{
						position = q;
						sub = q;
						//std::cout << "I am q " << q << std::endl; 
						break;
					}
				}
				int leftDist = abs(Devs[i].headPos - Devs[i].devQ[0]->cyLoc);
				//std::cout << "I am leftDist " << leftDist << std::endl; 
				int rightDist = abs(Devs[i].headPos - Devs[i].devQ[size - 1]->cyLoc);
				//std::cout << "I am rightDist " << rightDist << std::endl; 
				if (leftDist < rightDist)
				{
					for (unsigned int j = position; j < Devs[i].devQ.size(); j++)
					{
						Devs[i].swap(Devs[i].devQ[j], Devs[i].devQ[j - sub]);
						//std::cout << "Did a leftDist swap" << std::endl; 
						position++;
					}
					std::sort(Devs[i].devQ.begin() + (position - sub), Devs[i].devQ.end(), customGreat);
				}
				else
				{
					for (int j = position; j >= (sub / 2) + 1; j--)
					{
						Devs[i].swap(Devs[i].devQ[j], Devs[i].devQ[sub - j]);
						//std::cout << "Did a rightDist swap" << std::endl; 
					}
				}
				//it->headPos = it->devQ[size - 1]->cyLoc; 
			}
		}
	}
	return;
}

/********CPU Functions*************/
CPU::CPU():process(nullptr)
{

}

//Returns True if Queue is Empty
bool CPU::isEmpty()
{
	if (process == nullptr)
		return true;
	return false;
}

//Takes Process from Another Queue
void CPU::grabProcess(PCB* sent)
{
	if (sent == nullptr)
	{
		std::cout << "NO PROCESS GIVEN!" << std::endl;
		return;
	}
	else
	{
		process = sent;
		process->CPUCalls++;
		process->setBurst();
		return;
	}
}

//Terminates the Process inside the CPU
void CPU::endProcess()
{
	if (process == nullptr)
	{
		std::cout << "NO PROCESS IN CPU!" << std::endl;
		return;
	}
	COMP_PROC++;
	TOTALCPUTIME =  TOTALCPUTIME + process->CPUTime;
	PCB* del = process;
	process = nullptr;
	FREEMEM += (del->segNum * del->segSize);
	delete del;
	return;
}

PCB* CPU::getProcess()
{
	return process;
}
//Returns Current Process in the CPU
PCB* CPU::sendProcess()
{
	if (process == nullptr)
	{
		std::cout << "NO PROCESS IN CPU!" << std::endl;
		return nullptr;
	}
	PCB* send = process;
	process = nullptr;
	return send;
}

void CPU::systemCall(int devLoc, DevList &Devices)
{
	std::string input;
	int length = 0;
	int start = 0;
	char r_w;
	int slice;
	int cyl;

	while (true)
	{
		std::cout << "How Much of the Quantum has Passed in Milliseconds: ";
		getline(std::cin, input);
		std::stringstream myStream(input);
		if(myStream >> slice && slice <= QUANTUM && slice >= 0)
			break;
		std::cout << "Please Enter a Value between 0 and " << QUANTUM << std::endl;
	}
	process->CPUTime += slice;
	process->setBurst();

	if (process->fileName.empty())
	{
		//Get Name of File
		std::cout << "What is the File Name: ";
		getline(std::cin, input);
		process->fileName = input;

		//Get MEMSTART
		while (true)
		{
			std::cout << "What is the Starting Memory Location: ";
			getline(std::cin, input);
			std::stringstream myStream(input);
			if (myStream >> start)
				break;
			std::cout << "!!Invalid number!!" << std::endl;
		}
		process->startLoc = start;
	}

	while (true)
	{
		if (Devices.getDevType() == DevList::_Type::PRINTER)
		{
			r_w = 'w';
			break;
		}
		std::cout << "Is This A Read or Write ['r', 'w']: ";
		getline(std::cin, input);

		if (input.length() == 1)
		{
			r_w = input[0];
			if (tolower(r_w) == 'r' || tolower(r_w) == 'w')
				break;
			else
				std::cout << "Please Enter either R or W" << std::endl;
		}
	}
	process->action = tolower(r_w);
	if (process->action == 'w')
	{
		if (process->fileLen < 0)
		{
			while (true)
			{
				std::cout << "What is the File Length: ";
				getline(std::cin, input);
				std::stringstream myStream(input);
				if (myStream >> length)
					break;
				std::cout << "!!Invalid number!!" << std::endl;
			}
			process->fileLen = length;
		}
	}

	if (Devices.getDevType() == DevList::_Type::DISK && process->cyLoc < 0)
	{
		while (true)
		{
			std::cout << "Which Cylinder of this Disk is Needed: ";
			getline(std::cin, input);
			std::stringstream myStream(input);
			if (myStream >> cyl && cyl <= (Devices.getDev(devLoc).numCyl) - 1 && cyl >= 0)
				break;
			std::cout << "Please Enter a Number Between 0 and " << (Devices.getDev(devLoc).numCyl) - 1 << std::endl;
		}
		process->cyLoc = cyl;
	}
	//std::cout << "\nShould be inserting starting here\n";
	Devices.getDev(devLoc).devQ.push_back(sendProcess());
	//std::cout << "\nShould be inserted here\n";
	return;
}

void CPU::interrupt(std::deque<PCB*> &Ready)
{
	if (process == nullptr)
		std::cout << "There is No Process in CPU\n";
	else
	{
		process->CPUTime += QUANTUM;
		process->setBurst();
		Ready.push_back(sendProcess());
	}
	return;
}

void CPU::print()
{
	AVGCPUTIME = TOTALCPUTIME / ((COMP_PROC > 0) ? COMP_PROC : 1);
	std::cout << std::setw(8) << "\nCPU:";
	if(process != nullptr)
		process->print();
	std::cout << "\nTotal CPU Time: " << TOTALCPUTIME;
	std::cout << "\nTotal Completed Processes: " << COMP_PROC;
	std::cout << "\nAverage CPU Time: " << AVGCPUTIME;
	std::cout << std::endl;
	return;
}
#endif
