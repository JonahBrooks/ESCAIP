#pragma once
#include "Task.h"

using namespace BWAPI;
using namespace std;

class TaskList
{
private:
	Task* Front;
	int size;
	void addUnitToMap(UnitType unit);
	map<UnitType, int> unitMap; /* map that stores amounts of units that passed through this list, useful for TaskManager's inactive list */ 
public:
	TaskList(void);
	void addTask(Task* newTask);
	void addTask(string taskLine);
	void addMaps(map<UnitType,int> *toMap);
	Task* removeFront();
	Task* removeTask(Task* oldTask); /* deletes the task and returns a pointer to the previous task in the list */
	Task* getFront();
	Task* contains(int taskID);
	bool isEmpty();
	int getSize();
	~TaskList(void);
};
