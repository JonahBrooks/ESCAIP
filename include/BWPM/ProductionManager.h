#pragma once
#include "ProductionOrder.h"
#include "ProductionDetails.h"
#include <BWTM\TaskManager.h>
#include <BWWM\WorkerManager.h>
#include <sstream>

//// These shouldn't be here. Moving these elsewhere to prevent the apocalypse.
// #define incomePerMiner .038
// #define incomePerRefiner .073
// #define dependencyPriority 60
// #define expansionPriority 58
// #define unitPriority 55
// #define productionPriority 40
// #define supplyRatio .875

class ProductionManager
{
private:
	TaskManager* taskMan;
	WorkerManager* workMan;

	bool buildCurrentOrder;
	bool switchingOrdersWhenReady;
	//bool firstBuild;
	ProductionOrder *barracks, *tempBarracks;
	ProductionOrder *factory, *tempFactory;
	ProductionOrder *starport, *tempStarport;
	vector<Unit*> buildings;
	set<UnitType> requiredUnits;
	set<UnitType> switchUnits;
	map<UnitType, int> maintenanceMap;
	BWAPI::UnitType whichToBuild;
	int eta;
	int minsReq;
	int gasReq;
	int timeToProduce;
	int supplyLimit;
	int frameCount;
	int barracksCount;
	int factoryCount;
	int machineShopCount;
	int starportCount;
	int controlTowerCount;
	float mineralCostPerFrame;
	float gasCostPerFrame;
	float mineralIncomePerFrame;
	float gasIncomePerFrame;
	float acceptableMineralLosses;
	float acceptableGasLosses;


	bool exists(UnitType unit);
	bool actuallyExists(UnitType unit);
	void setIncomes();
	void manage();
	void resolveDependencies(bool temp);
	void recalculateSupply();
	void buildDependencies();
	void clearDependencies();
	void getDependencyCosts();
	void buildMoreProduction(bool temp);
	void setTimeAndCosts(bool temp);
	void produce(map<UnitType, int>::iterator c, bool &built);
	void doProduction();
	void doMaintenanceProduction();
	void doAddons();
	int getCurrentProductionTime(bool temp);
	int getTotalMineralCost(bool temp);
	int getTotalGasCost(bool temp);
	int resolveUnitDependency(UnitType unit);
	int getNumSupplyDepots();

public:
	ProductionManager(void);
	~ProductionManager(void);

	//int getFrameETA();
	void onFrame();
	void onUnitCreate(Unit* unit);
	void onUnitDestroy(Unit* unit);
	void onUnitComplete(Unit* unit);
	void prepareTechFor(UnitType unit);
	void prepareTechFor(UpgradeType ups);
	void prepareTechFor(TechType tech);
	void getAvailableUpgradesFor(UnitType unit);
	void maintainThisUnit(UnitType unit, int amount);
	void removeFromMaintenanceMap(UnitType unit);
	void expandHere(TilePosition tPos, bool ccNeeded);  
	void runOrderNow();
	void runOrderWhenReady();
	void setAcceptableLosses(float minLosses, float gasLosses);
	void resetAcceptableLosses();
	bool researchThis(UpgradeType ups);
	bool researchThis(TechType tech);
	bool produceThis(UnitType unit);
	map<UnitType,int> getMaintenanceMap();
	ProductionDetails loadNewOrder(ProductionOrder *newOrder);
};

