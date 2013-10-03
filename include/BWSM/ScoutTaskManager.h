#pragma once

#include <BWTM/TaskList.h>
#include "EnemyInfo.h"


class ScoutTaskManager
{
private:
	TaskList *current;
	TaskList *completed;
	int frameCount, start, interval;
	bool started;
	EnemyInfo* enemy;

	void manageTasks();
public:
	ScoutTaskManager(int startingScout, int scoutingInterval);
	~ScoutTaskManager(void);

	void addTask(EnemyRegion* place);
	void addTask(BWTA::Region* place);
	void onFrame();
	void onUnitDiscover(Unit* unit);
	void onUnitDestroy(Unit* unit);
	void onUnitEvade(Unit* unit);

	EnemyInfo* getEnemy();

};

