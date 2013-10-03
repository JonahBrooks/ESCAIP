#include "EnemyRegion.h"
#include <sstream>
typedef pair<Position,bool> scoutPair;

EnemyRegion::EnemyRegion(BWTA::Region* newRegion)
{
	percentageScoutedAtLastReset = 0;
	framesSinceLastScoutingReset = 0;
	region = newRegion;
	Polygon poly = region->getPolygon();
	scoutingPoints.push_back(scoutPair(region->getCenter(),false));
	int distance = int(scoutingPoints[0].first.getDistance(poly.getNearestPoint(scoutingPoints[0].first)) / 1.75f);
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x() + distance, scoutingPoints[0].first.y()),false));
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x() - distance, scoutingPoints[0].first.y()),false));
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x(), scoutingPoints[0].first.y() + distance),false));
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x(), scoutingPoints[0].first.y() - distance),false));
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x() + int(.7 * distance), scoutingPoints[0].first.y() - int(.7 * distance)),false));
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x() + int(.7 * distance), scoutingPoints[0].first.y() + int(.7 * distance)),false));
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x() - int(.7 * distance), scoutingPoints[0].first.y() - int(.7 * distance)),false));
	scoutingPoints.push_back(scoutPair(Position(scoutingPoints[0].first.x() - int(.7 * distance), scoutingPoints[0].first.y() + int(.7 * distance)),false));
	
	for(int c = 1; c < 9; c++)
	{
		Position newPos = poly.getNearestPoint(scoutingPoints[c].first);
		if(region->getCenter().x() > newPos.x())
			newPos.x() += 5;
		else
			newPos.x() -= 5;
		if(region->getCenter().y() > newPos.y())
			newPos.y() += 5;
		else
			newPos.y() -= 5;


		scoutingPoints.push_back(scoutPair(newPos,false));

	}
}


EnemyRegion::~EnemyRegion(void)
{
}

bool EnemyRegion::isInRange(Unit* unit, Position pos, int range)
{
	//Broodwar->printf("range: %d distance: %d", range, pos.getApproxDistance(unit->getPosition()));
	if(pos.getApproxDistance(unit->getPosition()) <= range)
		return true;
	return false;
}




void EnemyRegion::onUnitDiscover(Unit* unit)
{
	enemyBuildings.insert(unit);
	enemyBuildingCount[unit->getType()] ++;
}

void EnemyRegion::onUnitDestroy(Unit* unit)
{
	enemyBuildings.erase(unit);
	enemyBuildingCount[unit->getType()] --;
}

void EnemyRegion::onUnitEvade(Unit* unit)
{
	MissingEnemy me;
	me.unit = unit->getType();
	me.frame = Broodwar->getFrameCount();
	enemyArmy.insert(pair<Position,MissingEnemy>(unit->getPosition(),me));
}

void EnemyRegion::onFrame()
{
	framesSinceLastScoutingReset ++;

	//Debug stuff below here

	for(unsigned int c = 0; c < scoutingPoints.size(); c++)
		Broodwar->drawCircleMap(scoutingPoints[c].first.x(), scoutingPoints[c].first.y(), 2, Colors::Red, true);
	
	map<Position,MissingEnemy>::iterator i;
	for(i = enemyArmy.begin(); i != enemyArmy.end(); i++)
	{
		Broodwar->drawTextMap(i->first.x(), i->first.y(), i->second.unit.getName().c_str());
	}

	stringstream temp(stringstream::in | stringstream::out);
	map<UnitType, int>::iterator c;
	for(c = enemyBuildingCount.begin(); c != enemyBuildingCount.end(); c++)
		if((*c).second != 0)
			temp << (*c).first.getName() << " " << (*c).second << endl;
	Broodwar->drawTextMap(region->getCenter().x(), region->getCenter().y(), temp.str().c_str());
}

void EnemyRegion::checkScout(Unit* unit)
{
	int range = Broodwar->self()->sightRange(unit->getType());
	for(unsigned int c = 0; c < scoutingPoints.size(); c++)
	{
		if(scoutingPoints[c].second == false)
			if(isInRange(unit, scoutingPoints[c].first, range))
				scoutingPoints[c].second = true;
	}
	
	map<Position, MissingEnemy>::iterator c;
	for(c = enemyArmy.begin(); c != enemyArmy.end(); c++)
	{
		if(isInRange(unit, c->first, range))
			enemyArmy.erase(c);
	}
}

void EnemyRegion::resetScouting()
{
	percentageScoutedAtLastReset = getPercentageScouted();
	for(unsigned int c = 0; c < scoutingPoints.size(); c++)
		scoutingPoints[c].second = false;
	framesSinceLastScoutingReset = 0;
}

bool EnemyRegion::hasBuilding(Unit* building)
{
	if(enemyBuildings.find(building) != enemyBuildings.end())
		return true;
	return false;
}

float EnemyRegion::getMins()  //todo:worker accuracy estimates
{
	if(enemyBuildingCount[UnitTypes::Terran_Command_Center] > 0 || enemyBuildingCount[UnitTypes::Protoss_Nexus] > 0 || enemyBuildingCount[UnitTypes::Zerg_Hatchery] > 0)
	{
		if((*region->getBaseLocations().begin()) != NULL)
		{
			if( (*region->getBaseLocations().begin())->getMinerals().size() > 0)
				mineralIncomeEstimate = .038 * 2 * (*region->getBaseLocations().begin())->getMinerals().size();
		}
		else mineralIncomeEstimate = 0;

	}
	else mineralIncomeEstimate = 0;
	return mineralIncomeEstimate;
}

float EnemyRegion::getGas()
{
	gasIncomeEstimate = 3 * .073 * (enemyBuildingCount[UnitTypes::Terran_Refinery] + enemyBuildingCount[UnitTypes::Protoss_Assimilator] + enemyBuildingCount[UnitTypes::Zerg_Extractor]);

	return gasIncomeEstimate;
}

int EnemyRegion::framesSinceReset()
{
	return framesSinceLastScoutingReset;
}

int EnemyRegion::getPercentageScouted()
{
	int count = 0, size = scoutingPoints.size();
	for(int c = 0; c < size; c++)
	{
		if(scoutingPoints[c].second == true)
			count++;
	}

	return int( count / (float(size)) * 100);
}

int EnemyRegion::buildingCount(UnitType unit)
{
	return enemyBuildingCount[unit];
}

BWTA::Region* EnemyRegion::getRegion()
{
	return region;
}

Position EnemyRegion::getScoutPosition()
{
	if(scoutingPoints[0].second == false)
		return scoutingPoints[0].first;
	int largdist = 0, largidx = 0, thisdist = 0;
	for(unsigned int c = 0; c < scoutingPoints.size(); c++)
	{
		if(scoutingPoints[c].second == false)
		{
			thisdist = 0;
			for(unsigned int i = 0; i < scoutingPoints.size(); i++)
				if(scoutingPoints[i].second == true)
					thisdist += scoutingPoints[c].first.getApproxDistance(scoutingPoints[i].first);
			if(thisdist > largdist)
			{
				largdist = thisdist;
				largidx = c;
			}
		}
	}
	return scoutingPoints[largidx].first;
}

map<UnitType,int> EnemyRegion::getBuildingAmounts()
{
	return enemyBuildingCount;
}

map<Position, MissingEnemy> EnemyRegion::getMissing()
{
	return enemyArmy;
}