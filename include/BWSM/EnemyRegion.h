#pragma once

#include <BWAPI.h>
#include <BWTA.h>
using namespace BWAPI;
using namespace BWTA;
using namespace std;

struct MissingEnemy
{
	UnitType unit;
	int frame;
};

class EnemyRegion /* holds data about each region the enemy player has buildings in */
{
private:
	BWTA::Region *region;
	set<Unit*> enemyBuildings;
	map<UnitType, int> enemyBuildingCount;
	map<Position, MissingEnemy> enemyArmy;
	vector<pair<Position, bool>> scoutingPoints;

	int framesSinceLastScoutingReset;
	int percentageScoutedAtLastReset;
	float mineralIncomeEstimate;
	float gasIncomeEstimate;

	bool isInRange(Unit* unit, Position pos, int range);
public:
	EnemyRegion(BWTA::Region* newRegion);
	~EnemyRegion(void);

	void onUnitDiscover(Unit* unit); /* only called on buildings in this region */
	void onUnitDestroy(Unit* unit); /* only called on buildings in this region */
	void onUnitEvade(Unit* unit); /* only called on units in this region */
	void onFrame();
	void checkScout(Unit* unit);
	void resetScouting();
	bool hasBuilding(Unit* building);
	float getMins();
	float getGas();
	int framesSinceReset();
	int getPercentageScouted();
	int buildingCount(UnitType unit);
	BWTA::Region* getRegion();
	Position getScoutPosition();
	map<UnitType,int> getBuildingAmounts();
	map<Position, MissingEnemy> getMissing();

};

