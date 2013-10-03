#pragma once
#include <BWAPI.h>
#include <map>
using namespace std;
using namespace BWAPI;
typedef pair<UnitType,int> unitPair;
class ProductionOrder
{
private:
	//int numFacilities;
	std::map<BWAPI::UnitType,int> totalProductionTime; // Maps facilities and add-ons to time required to finish production
	int totalMineralCost;
	int totalGasCost;
	map<UnitType, int> units;

public:
	ProductionOrder(void);
	~ProductionOrder(void);

	void clearOrder();
	void addToOrder(UnitType unit, int amount);
	int getProductionTime(BWAPI::UnitType unit);
	int getMineralCost();
	int getGasCost();
	//void facilityIncrement();
	//void facilityDecrement();
	//void setFacilities(int newFacilities);
	map<UnitType, int>* getUnits();
};

