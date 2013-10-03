#pragma once
#include "Refinery.h"

class MiningBase
{
private:
	static vector<Unit*>* workers;
	BaseLocation* location;
	Refinery* refinery;
	float minersPerPatch;
	bool active;
	set<Unit*> miners;
	int patchCount;
public:
	MiningBase(BaseLocation* newLocation);
	~MiningBase(void);

	static void init(vector<Unit*>* works);
	void manageBase();
	void sendMiner(Unit* newMiner);
	void setMPP(float newMPP);
	void setGas(int newGas);
	void setSomeIdle();
	int getApproxMiners();
	int getMaxMinerCount();
	int getDistanceFrom(Position fromPosition);
	int getNumPatches();
	bool isActive();
	bool isEmpty();
	bool hasGasAvailable();
	bool hasRefinery();
	bool isGasSaturated();
	TilePosition getGasPosition();
};

