#pragma once
#include "MiningBase.h"
class WorkerManager
{
private:
	vector <Unit*> workers; /* All living workers */
	vector <MiningBase*> bases; /* All bases, active or inactive */
	float defaultMPP;
	bool saturated;
	int frameCount;
public:
	WorkerManager(void);
	~WorkerManager(void);

	void setDefaultMPP(float newMPP);
	void addWorker(Unit* newWorker);
	void removeWorker(Unit* deadWorker);
	void manage();
	int getWorkerMax();
	int getAdjustedMinerCount();
	int getAdjustedRefinerCount();
	TilePosition getNewGasPosition();
	bool canShiftToGas();
	bool canShiftToMins();
};

