#pragma once

#include <BWAPI.h>
#include <BWTA.h>
#include <windows.h>
#include <sstream>
#include "BuildingPlacer.h"
#include <BWSM\EnemyRegion.h>


using namespace BWAPI;
using namespace std;

enum Precon
{
	WORKERS,
	SECONDS,
	TASK_FINISHED,
	TASK_IN_PROGRESS,
	SUPPLY
};



class Task
{
protected:
	int preconditionAmount;
	Precon precondition;
	int xPos, yPos, taskNum, priority;  /* All tasks have pixel position accuracy, not tile position */
	Unit* worker;
	Unit* buildUnit;
	bool has_Worker, has_buildUnit, is_Wall;
	static vector<Unit*> workers;
	static vector<Unit*> buildings;
	static BuildingPlacer *placer;

	TilePosition findNewBuildPosition(TilePosition tPos, UnitType unit);
	void setPosition(TilePosition tPos);
	
public:
	
	static void init();
	Task* next;
	Task* prev;
	Task(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority);
	~Task(void);

	static void checkDestroyed(Unit* destroyedUnit); 
	static void addUnit(Unit* newUnit);
	virtual string getName() = 0;
	virtual void execute(int &mins, int &gas) = 0;
	virtual void cancel() = 0;
	virtual Task* makeNew() = 0;
	virtual int checkStatus() = 0;

	void setWorker(Unit* newWorker);
	void setBuildUnit(Unit* newBuildUnit);
	void setWall();
	int getSupply();
	int getTaskNum();
	int getPriority();
	Precon getPrecondition();
	Unit* getWorker();
	Unit* getBuildUnit();
	bool hasWorker();
	bool hasBuildUnit();
	bool isWall();
};

class BuildingTask : public Task
{
private:
	UnitType build;
	Unit* getWorker();
public:
	BuildingTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority);
	void execute(int &mins, int &gas);
	void cancel();
	Task* makeNew();
	int checkStatus();
	string getName();
};

class UnitTask : public Task
{
private:
	bool active;
	UnitType unit;
	Unit* getWorker();
public:
	UnitTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority, bool isTraining);
	void execute(int &mins, int &gas);
	void cancel();
	Task* makeNew();
	int checkStatus();
	string getName();
};

class AddonTask : public Task
{
private:
	UnitType addon;
	Unit* getWorker();
public:
	AddonTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority);
	void execute(int &mins, int &gas);
	void cancel();
	Task* makeNew();
	int checkStatus();
	string getName();
};

class TechTask : public Task
{
private:
	bool active;
	TechType tech;
	Unit* getWorker();
public:
	TechTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority, bool isResearching);
	void execute(int &mins, int &gas);
	void cancel();
	Task* makeNew();
	int checkStatus();
	string getName();
};

class UpgradeTask : public Task
{
private:
	bool active;
	UpgradeType ups;
	Unit* getWorker();
public:
	UpgradeTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority, bool isUpgrading);
	void execute(int &mins, int &gas);
	void cancel();
	Task* makeNew();
	int checkStatus();
	string getName();
};

class ExpandTask : public Task
{
private:
	Unit* getWorker();
public:
	ExpandTask(int x, int y);
	void execute(int &mins, int &gas);
	void cancel();
	Task* makeNew();
	int checkStatus();
	string getName();
};

class ScoutTask : public Task
{
private:
	Unit* getWorker();
	EnemyRegion* region;
	int scanCooldown;
	bool hostileRegion;
public:
	ScoutTask(EnemyRegion* place);
	void execute(int &mins, int &gas);
	void cancel();
	Task* makeNew();
	int checkStatus();
	string getName();
};


