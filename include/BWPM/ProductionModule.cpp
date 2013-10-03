#pragma once

#include <BWPM\ProductionModule.h>

ProductionManager *ProductionModule::singleton;
ProductionOrder *ProductionModule::order;

void ProductionModule::init() { 
	Task::init();
	
	static ProductionManager *manager = new ProductionManager();
	static ProductionOrder *newOrder = new ProductionOrder();
	singleton = manager;
	order = newOrder;
	
}
	
void ProductionModule::onFrame() { 	
	singleton->onFrame();
}

void ProductionModule::onUnitCreate(Unit* unit) { 	
	singleton->onUnitCreate(unit);
}

void ProductionModule::onUnitDestroy(Unit* unit) { 	
	singleton->onUnitDestroy(unit);
}

void ProductionModule::onUnitComplete(Unit* unit) { 	
	singleton->onUnitComplete(unit);
}

void ProductionModule::prepareTechFor(UnitType unit) { 	
	singleton->prepareTechFor(unit);
}

void ProductionModule::prepareTechFor(UpgradeType ups) { 	
	singleton->prepareTechFor(ups);
}

void ProductionModule::prepareTechFor(TechType tech) { 	
	singleton->prepareTechFor(tech);
}

void ProductionModule::getAvailableUpgradesFor(UnitType unit) { 	
	singleton->getAvailableUpgradesFor(unit);
}

void ProductionModule::maintainThisUnit(UnitType unit, int amount) { 	
	singleton->maintainThisUnit(unit, amount);
	singleton->loadNewOrder(order);
	singleton->runOrderWhenReady();
}

void ProductionModule::removeFromMaintenanceMap(UnitType unit) { 	
	singleton->removeFromMaintenanceMap(unit);
}

void ProductionModule::expandHere(TilePosition tPos, bool ccNeeded) { 	
	singleton->expandHere(tPos, ccNeeded);
}

void ProductionModule::addToOrder(UnitType unit, int amount) {
	order->addToOrder(unit, amount);
	singleton->loadNewOrder(order);
	singleton->runOrderWhenReady();
}

void ProductionModule::setAcceptableLosses(float minLosses, float gasLosses) { 	
	singleton->setAcceptableLosses(minLosses, gasLosses);
}

void ProductionModule::resetAcceptableLosses() { 	
	singleton->resetAcceptableLosses();
}

bool ProductionModule::upgrade(UpgradeType ups, bool prepareTech) {
	if (prepareTech)
	{
		singleton->prepareTechFor(ups);
	}
	return singleton->researchThis(ups);
}

bool ProductionModule::research(TechType tech, bool prepareTech) {
	if (prepareTech)
	{
		singleton->prepareTechFor(tech);
	}
	return singleton->researchThis(tech);
}

bool ProductionModule::produceThis(UnitType unit) { 	
	return singleton->produceThis(unit);
}

map<UnitType,int> ProductionModule::getMaintenanceMap() { 	
	return singleton->getMaintenanceMap();
}

