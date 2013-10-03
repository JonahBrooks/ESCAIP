#pragma once

#include <BWSM\ScoutingModule.h>

ScoutTaskManager *ScoutingModule::manager;
EnemyInfo *ScoutingModule::info;

void ScoutingModule::init() { 
	delete manager;
	manager = new ScoutTaskManager(5000, 1440);
	info = manager->getEnemy();
}

void ScoutingModule::addTask(BWTA::Region* place) { 
	manager->addTask(place);
}

void ScoutingModule::onFrame() { 
	manager->onFrame();
}

void ScoutingModule::onUnitDiscover(Unit* unit) { 
	manager->onUnitDiscover(unit);
}

void ScoutingModule::onUnitDestroy(Unit* unit) { 
	manager->onUnitDestroy(unit);
}

void ScoutingModule::onUnitEvade(Unit* unit) { 
	manager->onUnitEvade(unit);
}

void ScoutingModule::resetScouting() { 
	return info->resetScouting();
}

float ScoutingModule::getMins() { 
	return info->getMins();
}

float ScoutingModule::getGas() { 
	return info->getGas();
}

int ScoutingModule::getFirstDiscoveryTime(BWAPI::UnitType) { 
	return 1; 
	// TODO: I'm going to have to iterate through all regions for the global minimum time?
}

int ScoutingModule::getBuildingAmount(BWAPI::UnitType) { 
	return 1;
	// TODO: Sum up the building count in all regions?
}



//TODO: ... maybe?

// startScouting() ?
// stopScouting() ?

// adjustInterval() ?

// getUnitCount() ?
// getArmyPosition() ?
// getWeakPoints() ?

// map<UnitType,int> getUnitEstimates() ?

// int getProductionTime(UnitType unit) ?

// int getPercentageScouted() ?

// Some way to iterate through regions ?

