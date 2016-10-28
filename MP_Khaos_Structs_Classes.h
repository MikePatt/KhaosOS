/***************************************
*	AUTHOR: Michael Patterson
*	CREATED: 11/15/15
*	PURPOSE: Khaos Operating System: Simulated operating system with a number of PRINTERs, DISKs, and CDROMs.
*	PROF: Eric Schweitzer
*	EDITS: NONE
*****************************************/
#ifndef KHAOS_STRUCTS_CLASSES_H
#define KHAOS_STRUCTS_CLASSES_H

#include "MP_Khaos_Global.h"

#include <stdio.h>
#include <vector>
#include <deque>
#include <string>
#include <utility>

struct PCB
{
	int pid;
	std::string fileName;
	int startLoc;
	char action;
	int fileLen;
	PCB* next = nullptr;

	
	// total time process spent in CPU
	int CPUTime = 0;
	//Number of Times entered CPU
	int CPUCalls = 0;
	//Average Time Spent in CPU
	float AvgBurstTime;
	//position of cylinders in disk
	int cyLoc = -1;

	int baseReg;
	int segNum;
	int segSize;

	PCB();
	PCB(int&);
	void setBurst()
		{AvgBurstTime = (float)CPUTime / (float)((CPUCalls > 0) ? CPUCalls : 1);}
	void print();
	void printDisk();
	int getPhyLoc(std::vector<int, int>);
};

class Memory
{
	private:
		std::vector<std::pair<int, int>> physAdd;
		std::vector<std::pair<int, int>> holes;
		std::vector<PCB*> inMem;
	public:
		Memory(int);
		void allocate(PCB*);
		void compact();
		void free(PCB*);
		void printMem();
		bool memCheck(PCB*);
		std::vector<std::pair<int, int>> getPhysAdd()
			{return physAdd;}
};

struct
{
	bool operator()(PCB* i, PCB* j)
	{
		return (i->cyLoc < j->cyLoc);
	}
} customLess;

struct
{
	bool operator()(PCB* i, PCB* j)
	{
		return (i->cyLoc > j->cyLoc);
	}
} customGreat;

//Queue Class Which Contains a Linked List of PCBs and Ways of Creating, Deleting, Sending, and Adding Processes 
/*
class Queue
{
	private:
		PCB* head = nullptr;
		PCB* tail = nullptr;
		int size = 0;
		int numCyl = 0;
		int headCyl = 0;

	public:
		//Create a Queue
		Queue();
		//Delete a Queue
		~Queue();
		//Create a Process and Add it to the Queue
		void addProcess(int&);
		//Delete a Process from the Queue
		void endProcess();
		//Send a Process to another Queue or CPU
		PCB* dequeue();
		//Accept a Process from another Queue or CPU
		bool enqueue(PCB*);
		//Print what is on the queue.
		void print();
		//Returns true if Queue is Empty.
		bool isEmpty();
		//return the num of cylinders
		int getNumCyl()
			{return numCyl;}
		// Set the number of cylinders for each disk
		void setNumCyl(int num)
			{numCyl = num;}
		//get the position of the disk head
		int getHeadCyl()
			{return headCyl;}
		//set the position of the disk head
		void setHeadCyl(int num)
			{headCyl = num;}
		//New Heuristic Disk Scheduling Algorithm
		void NHDSA(Queue&, int, int);

		// Sorts the Queue
		Queue sort(const Queue&);

};
*/

//DevList Contains an Array of Queues for Each Device and the Type of Device
class DevList
{
	public:
		enum _Type
		{
			PRINTER = 0,
			DISK = 1,
			CDRW = 2
		};

	private:
		struct Device
		{
			std::deque<PCB*> devQ;
			int headPos = 0;
			int numCyl;

			void swap(PCB*, PCB*);
			void print(_Type);
		};

		std::vector<Device> Devs;
		_Type devType;

		public:
			DevList();
			DevList(int, _Type);
			void interrupt(int, std::deque<PCB*>&);
			void print();
			_Type getDevType()
				{return devType;}
			int getDevNum()
				{return Devs.size();}
			Device& getDev(int pos)
				{return Devs[pos - 1];}
			void NHDSA();
};

//The CPU Class Which Contains a Process, and Methods to Take One, Send One, and Delete One
class CPU
{
	private:
		PCB* process;

	public:
		CPU();
		bool isEmpty();
		void grabProcess(PCB*);
		PCB* getProcess();
		void endProcess();
		PCB* sendProcess();
		void systemCall(int, DevList&);
		void interrupt(std::deque<PCB*>&);
		void print();
};
#endif
