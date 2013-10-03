#include "ProductionOrder.h"


ProductionOrder::ProductionOrder(void)
{
//	numFacilities = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Barracks] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Factory] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Machine_Shop] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Starport] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Control_Tower] = 0;
	totalMineralCost = 0;
	totalGasCost = 0;
}

ProductionOrder::~ProductionOrder(void)
{
}

void ProductionOrder::clearOrder()
{
	units.clear();
	totalProductionTime[BWAPI::UnitTypes::Terran_Barracks] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Factory] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Machine_Shop] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Starport] = 0;
	totalProductionTime[BWAPI::UnitTypes::Terran_Control_Tower] = 0;
	totalMineralCost = 0;
	totalGasCost = 0;
}

void ProductionOrder::addToOrder(UnitType unit, int amount)
{
	units[unit] += amount;
	totalMineralCost += unit.mineralPrice() * amount;
	totalGasCost += unit.gasPrice() * amount;
	totalProductionTime[unit.whatBuilds().first] += unit.buildTime() * amount;
	
	// TODO: Make a function that looks this up in a predefined table. It's wasteful to recalculate each time.
	for(std::map<BWAPI::UnitType,int>::const_iterator req = unit.requiredUnits().begin(); req != unit.requiredUnits().end(); req++)
	{
		// Find any addons (for production facilities) that are required to build this unit
		if((*req).first.isAddon() && (*req).first.whatBuilds().first.canProduce())
		{
			totalProductionTime[(*req).first] += unit.buildTime() * amount;
		}
	}

}

int ProductionOrder::getProductionTime(BWAPI::UnitType unit)
{
	return totalProductionTime[unit];
}

int ProductionOrder::getMineralCost()
{
	return totalMineralCost;
}

int ProductionOrder::getGasCost()
{
	return totalGasCost;
}

//void ProductionOrder::facilityIncrement()
//{
//	numFacilities ++;
//}
//
//void ProductionOrder::facilityDecrement()
//{
//	numFacilities --;
//}

//void ProductionOrder::setFacilities(int newFacilities)
//{
//	numFacilities = newFacilities;
//}

map<UnitType, int>* ProductionOrder::getUnits()
{
	return &units;
}