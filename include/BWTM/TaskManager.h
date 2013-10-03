#pragma once
#include "TaskList.h"
#include "usefulThings.h"
#include "BuildingPlacer.h"
#include <fstream>
#include <sstream>

using namespace BWAPI;
using namespace std;

#define workerPriority 65

class TaskManager
{
private:

	map<UnitType, int> activeUnits;
	map<UnitType, int> inactiveUnits;
	BuildingPlacer placer;

	/* These vectors hold pointers to units that the AI owns */
	vector<Unit*> buildings; /* Holds all of the buildings that the AI owns */
	
	/* These TaskLists hold all of the tasks currently given to the task manager */
	TaskList* inactive; 
	TaskList* waiting;
	TaskList* active;
	TaskList* completed;

	int workerCount;
	int maxWorkers;

	bool prereqCheck(Task* task); /* Checks if the preconditions for a task are met, to be moved from the inactive queue to the waiting queue */
	void manageInactiveTasks(); /* Checks for tasks that may be moved to the waiting queue */
	void manageWaitingTasks(); /* manages the completion of tasks in the waiting queue */
	void manageActiveTasks(); /* Checks for the completion of tasks in the active queue */
	void scrapTaskFromList(UnitType unit, TaskList *list); /* Scraps task of UnitType unit from the given TaskList */
	void getTasksFromList(set<UnitType> &tasks, TaskList *list); /* Adds the unit types from the given task list to the set of tasks */
	Task* addToWaitingQueue(Task* newTask); /* Decides if the task should be added to the waiting list, if so then adds it */
	int getNumWaiting(string unitName);

public:
	TaskManager(char* fileName);
	~TaskManager(void);

	//static vector<Unit*> getWorkers(){return workers;};
	//static vector<Unit*> getBuildings(){return buildings;};

	void setWorkerMax(int newMax);
	void checkDestroyed(Unit* destroyedUnit); /* Should be called in onUnitDestroy(), removes dead worker from workers vector and destroyed buildings from buildings vector */
	void addUnit(Unit* newUnit); /* should be called in onUnitCreate(), adds unit to workers or buildings vector if necessary */
	void manageTasks(); /* should be called onFrame(), deals with tasks and default unit creation */
	void readFile(char* fileName); /* reads in a list of tasks from a .txt file */ 
	
	/* adds a single task from a string input. A quick description of the task interpreter:
		*Required* 
		Name of the unit
			The name of the unit or tech type must be placed in triangle brackets, eg <Terran Marine>
		*Optional*
		Amounts
			Add a comma after the unit name then include an integer amount, within the brackets. eg <Terran Marine,5>
		Task Numbers
			Task numbers can be assigned after a # with a following space, eg Task 30 would need "#30 ". These are used in Preconditions
		Preconditions
			Preconditions can be placed in parentheses, and can be used to start a task when the ai hits a certain number of workers, 
			starts a certain task, finishes a certain task, or when the game has run for a specified number of seconds. Preconditions
			require an integer followed by a colon, then 'w' for workers, 'p' for a task in progress, 'f' for a finished task, 's' for seconds, and
			't' for seconds. eg 12 SCVs = (12:w), 15 supply = (15:s), Task 32 in progress = (32:p), Task 10 finished = (10:f), 500 seconds have passed = (500:t)
		Coordinates
			Place pixel coordinates in square brackets, eg [352 1012] or [400,764]. Approximate coordinates are acceptable, building coordinates
			do not have to be valid
	*/
	void addTask(string newTask);
	void addUnitTask(UnitType unit, int amount, int priority = 50);
	void addUnitTask(UnitType unit, TilePosition tPos, int priority = 50);
	void addTechTask(TechType tech, int priority = 50);
	void addUpgradeTask(UpgradeType ups, int priority = 50);
	void addExpansionTask(TilePosition tPos, int priority = 50);
	int getActiveUnits(UnitType unit);
	int getInactiveUnits(UnitType unit);
	void clearFromActiveMap(UnitType unit, int amount);
	void expand(int x, int y);
	bool expandTaskExists();

	void scrapTask(UnitType unit); /* scraps all current incomplete tasks of the given UnitType */
	set<UnitType> getPendingTasks(); /* Returns the set of unit types to be built by the manager */
	set<UnitType> getCompletedTasks(); /* Returns the set of all unit types completed by the manager */

};

