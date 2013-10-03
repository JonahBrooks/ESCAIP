#pragma once
#include "EnemyRegion.h"

class EnemyInfo /* stores all data collected from onUnitDiscover() */
{
private:
	Player* enemyPlayer;
	vector<EnemyRegion*> regions;
	map <UnitType,int> timeFirstDiscovered;
	map <UnitType,int> buildingAmounts;
	map <Position, MissingEnemy> missingUnits;
	float estMineralIncome;
	float estGasIncome;

	int frameCount;

	//not sure how to store enemy army data

	bool buildingAlreadyFound(Unit* building);
	bool regionExists(BWTA::Region* region);
	void collectData(); /* collect data from regions and scouts */
	void checkScouting(Unit* unit);
	EnemyRegion* getEnemyRegion(Unit* unit);

public:
	EnemyInfo(BWTA::Region* enemyStart);
	~EnemyInfo(void);

	void onUnitDiscover(Unit* unit);
	void onUnitDestroy(Unit* unit);
	void onFrame();
	void onUnitEvade(Unit* unit);
	void resetScouting();
	float getMins();
	float getGas();
	vector<EnemyRegion*> getRegions();
	map<UnitType,int> getDiscoveryTimes();
	map<UnitType,int> getBuildingAmounts();
	map<Position, MissingEnemy> getMissingUnits();
	Player* getEnemy();
	//bool hasUnit(UnitType unit);
	//bool couldHaveUnit(UnitType unit);
};

