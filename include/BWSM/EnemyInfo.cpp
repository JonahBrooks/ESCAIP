#include "EnemyInfo.h"


EnemyInfo::EnemyInfo(BWTA::Region* enemyStart)
{
	estMineralIncome = 0;
	estGasIncome = 0;
	frameCount = 0;
	regions.push_back(new EnemyRegion(enemyStart));
	enemyPlayer = Broodwar->enemy();

	set<BWTA::Region*> regs = BWTA::getRegions();
	set<BWTA::Region*>::iterator c;
	for(c = regs.begin(); c != regs.end(); c++)
	{
		if((*c) != enemyStart)
			regions.push_back(new EnemyRegion(*c));
	}
}


EnemyInfo::~EnemyInfo(void)
{
}

bool EnemyInfo::buildingAlreadyFound(Unit* unit)
{
	for(unsigned int c = 0; c < regions.size(); c ++)
	{
		if(regions[c]->hasBuilding(unit))
			return true;
	}
	return false;
}

bool EnemyInfo::regionExists(BWTA::Region* region)
{
	for(unsigned int c = 0; c < regions.size(); c++)
	{
		if(regions[c]->getRegion() == region)
			return true;
	}
	return false;
}

void EnemyInfo::collectData()
{
	//check scouting data for all units
	set<Unit*>units = Broodwar->self()->getUnits();
	set<Unit*>::iterator c;
	for(c = units.begin(); c != units.end(); c++)
	{
		if(!(*c)->isGatheringMinerals())
		{
			checkScouting(*c);
		}
	}

	estMineralIncome = 0;
	estGasIncome = 0;
	buildingAmounts.clear();
	missingUnits.clear();
	map<UnitType,int>::iterator j;
	map<UnitType,int> regionUnits;
	map<Position,MissingEnemy>::iterator k;
	map<Position,MissingEnemy> regionMiss;
	for(unsigned int i = 0; i < regions.size(); i++)
	{
		estMineralIncome += regions[i]->getMins();
		estGasIncome += regions[i]->getGas();
		regionUnits = regions[i]->getBuildingAmounts();
		for(j = regionUnits.begin(); j != regionUnits.end(); j++)
			buildingAmounts[j->first] += j->second;
		regionMiss = regions[i]->getMissing();
		for(k = regionMiss.begin(); k != regionMiss.end(); k++)
		{
			missingUnits[k->first] = k->second;
		}
	}
	for(j = timeFirstDiscovered.begin(); j != timeFirstDiscovered.end(); j++)
	{
		if(buildingAmounts[j->first] == 0)
			timeFirstDiscovered.erase(j->first);
	}
}

void EnemyInfo::checkScouting(Unit* unit)
{
	EnemyRegion* region = getEnemyRegion(unit);
	if(region != NULL)
	{
		region->checkScout(unit);
	}
}


EnemyRegion* EnemyInfo::getEnemyRegion(Unit* unit)
{
	BWTA::Region* bwta = NULL;
	for(unsigned int i = 0; i < regions.size(); i++)
	{
		bwta = regions[i]->getRegion();
		if(bwta->getPolygon().isInside(unit->getPosition()))
			return regions[i];
	}
	return NULL;
}



void EnemyInfo::onUnitDiscover(Unit* unit)
{
	if(unit->getPlayer() == enemyPlayer && unit->getType().isBuilding() && (!unit->getType().isResourceContainer() || (unit->getType() == UnitTypes::Terran_Refinery ||  unit->getType() == UnitTypes::Zerg_Extractor || unit->getType() == UnitTypes::Protoss_Assimilator)))
	{
		if(timeFirstDiscovered[unit->getType()] == 0 || timeFirstDiscovered[unit->getType()] > Broodwar->getFrameCount() + unit->getRemainingBuildTime())
			timeFirstDiscovered[unit->getType()] = Broodwar->getFrameCount() + unit->getRemainingBuildTime();
		if(!buildingAlreadyFound(unit))
		{
			if(!regionExists(getRegion(unit->getTilePosition())))
			{
				//Broodwar->printf("MOAR ONE REGION");
				Broodwar->printf("%s",unit->getType().getName().c_str());
				regions.push_back(new EnemyRegion(getRegion(unit->getTilePosition())));
			}
			getEnemyRegion(unit)->onUnitDiscover(unit);
		}
	}
}

void EnemyInfo::onUnitDestroy(Unit* unit)
{
	if(unit->getPlayer() == enemyPlayer && unit->getType().isBuilding())
	{
		if(buildingAlreadyFound(unit))
		{
			getEnemyRegion(unit)->onUnitDestroy(unit);
		}
	}
}

void EnemyInfo::onFrame()
{
	frameCount ++;
	if(frameCount > 24)
	{
		frameCount = 0;
		collectData();
	}
	for(unsigned int c = 0; c < regions.size(); c++)
		regions[c]->onFrame();
}

void EnemyInfo::onUnitEvade(Unit* unit)
{
	if(!unit->getType().isBuilding() && !unit->getType().isWorker() && unit->getPlayer() == enemyPlayer)
	{
		EnemyRegion* region = getEnemyRegion(unit);
		if(region != NULL)
		{
			region->onUnitEvade(unit);
		}
	}
}

void EnemyInfo::resetScouting()
{
	for(unsigned int c = 0; c < regions.size(); c++)
	{
		if(regions[c]->getPercentageScouted() > 0)
			regions[c]->resetScouting();
	}
}

float EnemyInfo::getMins()
{
	return estMineralIncome;
}

float EnemyInfo::getGas()
{
	return estGasIncome;
}

vector<EnemyRegion*> EnemyInfo::getRegions()
{
	return regions;
}

map<UnitType, int> EnemyInfo::getDiscoveryTimes()
{
	return timeFirstDiscovered;
}

map<UnitType, int> EnemyInfo::getBuildingAmounts()
{
	return buildingAmounts;
}

map<Position, MissingEnemy> EnemyInfo::getMissingUnits()
{
	return missingUnits;
}

Player* EnemyInfo::getEnemy()
{
	return enemyPlayer;
}