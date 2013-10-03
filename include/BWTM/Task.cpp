#include "Task.h"
vector<Unit*> Task::workers;
vector<Unit*> Task::buildings;
BuildingPlacer* Task::placer = NULL;
//int costMins = 0;
//int costGas = 0;
extern Unit* baseWall;

Task::Task(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority)
{
	preconditionAmount = supplyCount;
	xPos = x;
	yPos = y;
	has_Worker = false;
	has_buildUnit = false;
	is_Wall = false;
	priority = newPriority;
	worker = 0;
	buildUnit = 0;
	precondition = pre;
	taskNum = taskID;
}

TilePosition Task::findNewBuildPosition(TilePosition tPos, UnitType unit)
{
	TilePosition position = placer->getBuildLocationNear(tPos, unit);
	placer->reserveTiles(position, unit.tileWidth(), unit.tileHeight());
	if(unit == UnitTypes::Terran_Factory || unit == UnitTypes::Terran_Starport || unit == UnitTypes::Terran_Science_Facility)
	{
		TilePosition addPos = TilePosition(position.x() + 3, position.y() + 1);
		placer->reserveTiles(addPos, 2, 1);
	}
	return position;
}

void Task::init()
{
	BuildingPlacer *newPlacer = new BuildingPlacer();
	placer = newPlacer;
}

void Task::addUnit(Unit* newUnit)
{
	if(newUnit->getPlayer() != Broodwar->self())
		return;
	if(newUnit->getType().isWorker())
	{
		workers.push_back(newUnit);
		return;
	}
	if(newUnit->getType().isBuilding())
	{
		buildings.push_back(newUnit);
		return;
	}
}

void Task::checkDestroyed(Unit* destroyedUnit)
{
	if(destroyedUnit->getPlayer() == Broodwar->self() && (destroyedUnit->getType().isWorker()))
		for(unsigned int c = 0; c < workers.size(); c++)
			if(workers[c] == destroyedUnit)
			{
				workers.erase(workers.begin() + c);
			}
	if(destroyedUnit->getPlayer() == Broodwar->self() && destroyedUnit->getType().isBuilding())
	{
		for(unsigned int c = 0; c < buildings.size(); c++)
			if(buildings[c] == destroyedUnit)
			{
				buildings.erase(buildings.begin() + c);
			}
	}
}

void Task::setPosition(TilePosition tPos)
{
	xPos = tPos.x() * 32;
	yPos = tPos.y() * 32;
}

void Task::setWorker(Unit* newWorker)
{
	worker = newWorker;
	has_Worker = true;
}

void Task::setBuildUnit(Unit* newBuildUnit)
{
	buildUnit = newBuildUnit;
	has_buildUnit = true;
}

void Task::setWall()
{
	is_Wall = true;
}

Precon Task::getPrecondition()
{
	return precondition;
}

int Task::getTaskNum()
{
	return taskNum;
}

int Task::getPriority()
{
	return priority;
}

int Task::getSupply()
{
	return preconditionAmount;
}

Unit* Task::getWorker()
{
	if(!worker)
		return 0;
	return worker;
}

Unit* Task::getBuildUnit()
{
	if(!buildUnit)
		return 0;
	return buildUnit;
}

bool Task::hasWorker()
{
	return has_Worker;
}

bool Task::hasBuildUnit()
{
	return has_buildUnit;
}

bool Task::isWall()
{
	return is_Wall;
}

Task::~Task(void)
{

}

//------------------------------------------------------Buildings-------------------------------------------//


BuildingTask::BuildingTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority)
	:Task(taskID, pre, unitName, supplyCount, x, y, newPriority)
{
	build = UnitTypes::getUnitType(unitName);
}

string BuildingTask::getName()
{
	return build.getName();
}

void BuildingTask::execute(int &mins, int &gas)
{
	//costMins += build.mineralPrice();
	//costGas += build.gasPrice();
	//Broodwar->printf("%s\n   %d,%d",getName().c_str(), mins, build.mineralPrice());

	//if(Broodwar->canMake(NULL,build) && mins >= build.mineralPrice() && gas >= build.gasPrice())
	if(Broodwar->canMake(NULL,build))
	{
		if(has_Worker && !worker->exists())
			has_Worker = false;
		if(!has_Worker)
		{
			worker = getWorker();
			if(worker != NULL)
				has_Worker = true;
		}

		if(has_Worker)
		{
			TilePosition tPos(xPos/32,yPos/32);
			Position pos(xPos,yPos);

			if((!Broodwar->canBuildHere(worker,tPos,build,false) && worker->getDistance(pos) < 100) || tPos.x() <= 0)
			{
				//Broodwar->printf("KKKKKKKKKKKKKKKKKKKKKKKKKKKK");
				if(is_Wall || build == UnitTypes::Terran_Supply_Depot)
					placer->setBuildDistance(0);
				placer->freeTiles(tPos,build.tileWidth(),build.tileHeight());

				if(build == UnitTypes::Terran_Factory || build == UnitTypes::Terran_Starport || build == UnitTypes::Terran_Science_Facility)
				{
					TilePosition addPos = TilePosition(tPos.x() + 3, tPos.y() + 1);
					placer->freeTiles(addPos, 2, 1);
				}

				if(tPos.x() <= 0)
					tPos = findNewBuildPosition(buildings[0]->getTilePosition(),build);
				else
					tPos = findNewBuildPosition(tPos,build);
				setPosition(tPos);
				if(is_Wall || build == UnitTypes::Terran_Supply_Depot)
					placer->setBuildDistance(1);
			}

			if(!Broodwar->canBuildHere(worker,tPos,build,true))
				worker->move(pos);
			else if(mins >= build.mineralPrice())
			{
				worker->build(tPos,build);
				//mins -= build.mineralPrice();
				//gas -= build.gasPrice();
			}	
		}
	}

	mins -= build.mineralPrice();
	gas -= build.gasPrice();
}

void BuildingTask::cancel()
{
	if(worker)
	{
		worker->cancelConstruction();
	}
	if(buildUnit)
		buildUnit->cancelConstruction();
}

Task* BuildingTask::makeNew()
{
	return new BuildingTask(taskNum, precondition, getName(), preconditionAmount, xPos, yPos, priority);
}

int BuildingTask::checkStatus()
{
	if(!has_Worker)
		return 0;
	if(worker)
	{
		if(!has_buildUnit)
			if(worker->isConstructing() && worker->getBuildUnit())
			{
				if(worker->getBuildUnit()->exists())
				{
					if(worker->getBuildUnit()->getType() == build)
					{
						buildUnit = worker->getBuildUnit();
						has_buildUnit = true;			
						placer->freeTiles(buildUnit->getTilePosition(),build.tileWidth(),build.tileHeight());

						if(is_Wall && build == UnitTypes::Terran_Barracks)
							baseWall = buildUnit;

						return 1;
					}
					else
					{
						has_Worker = false;
						worker = NULL;
						return 0;
					}
				}
			}
	}

	if(has_buildUnit)
	{
		if(buildUnit)
		{
			if(buildUnit->getRemainingBuildTime() == 0)
				return 2;
			if(buildUnit->getRemainingBuildTime() > 0)
			{
				if(!worker)
					worker = getWorker();
				if(worker)
				{
					if(!worker->exists())
						worker = getWorker();
					else if(!worker->isConstructing())
						worker->rightClick(buildUnit);
				}
			}

		}
	}

	return 0;
}

Unit* BuildingTask::getWorker()
{
	int distance = 9999999;
	Unit* closest = NULL;
	Position target = Position(xPos,yPos);
	for(unsigned int c = 0; c < workers.size(); c++)
		if(!workers[c]->isConstructing() && workers[c]->getRemainingBuildTime() == 0 && workers[c]->exists() && !workers[c]->isGatheringGas() && workers[c]->isGatheringMinerals())
		{
			if(workers[c]->getDistance(target) < distance)
			{
				//Broodwar->printf("%d",workers[c]->getDistance(target));
				distance = workers[c]->getDistance(target);
				closest = workers[c];
			}
		}
	return closest;
}


//------------------------------------------------------Units-------------------------------------------//

UnitTask::UnitTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority, bool isActive)
	:Task(taskID, pre, unitName, supplyCount, x, y, newPriority)
{
	unit = UnitTypes::getUnitType(unitName);
	active = isActive;
}

string UnitTask::getName()
{
	return unit.getName();
}

void UnitTask::execute(int &mins, int &gas)
{
	//costMins += unit.mineralPrice();
	//costGas += unit.gasPrice();
	if(Broodwar->canMake(NULL,unit) && mins >= unit.mineralPrice() && gas >= unit.gasPrice())
	{
		if(!has_Worker)
		{
			worker = getWorker();
			if(worker != NULL)
				has_Worker = true;
		}

		if(has_Worker)
		{	
			if(!worker->isTraining())
			{
				if(worker->train(unit))
				{
					active = true;
					//mins -= unit.mineralPrice();
					//gas -= unit.gasPrice();
				}
			}
			else
			{
				has_Worker = false;
				worker = getWorker();
				if(worker != NULL)
					has_Worker = true;
			}	
		}
	}
	//if(Broodwar->canMake(NULL,unit) && unit != UnitTypes::Terran_SCV)
	if(unit != UnitTypes::Terran_SCV)
	{
		mins -= unit.mineralPrice();
		gas -= unit.gasPrice();
	}
}

void UnitTask::cancel()
{
	if(worker && active)
	{
		worker->cancelTrain();
	}
}

Task* UnitTask::makeNew()
{
	return new UnitTask(taskNum, precondition, getName(), preconditionAmount, xPos, yPos, priority, active);
}

int UnitTask::checkStatus()
{
	if(!has_Worker)
		return 0;
	if(active && worker->getRemainingTrainTime() > 0)
		return 1;
	if(active && worker->getRemainingTrainTime() == 0)
		return 2;
	return 0;
}

Unit* UnitTask::getWorker()
{
	Position target = Position(xPos,yPos);
	int distance = 9999999;
	Unit* closest = NULL;
	UnitType trainer = unit.whatBuilds().first;
	UnitType reqAddon = UnitTypes::None;
	Unit *addon = NULL;
	for(std::map<BWAPI::UnitType,int>::const_iterator req = unit.requiredUnits().begin(); req != unit.requiredUnits().end(); req++)
	{
		// Find any addons (for production facilities) that are required to build this unit
		if((*req).first.isAddon() && (*req).first.whatBuilds().first.canProduce())
		{
			reqAddon = (*req).first;
		}
	}
	
	for(unsigned int c = 0; c < buildings.size(); c++)
	{
		if(buildings[c] && buildings[c]->exists() && buildings[c]->getType() == trainer && !buildings[c]->isTraining())
		{
			addon = buildings[c]->getAddon();
			// requiredUnits, getAddon, isAddon
			if ( (addon == NULL && reqAddon == UnitTypes::None) || (addon != NULL && reqAddon == addon->getType()) )
			{	
				if(buildings[c]->getDistance(target) < distance)
				{
					distance = buildings[c]->getDistance(target);
					closest = buildings[c];
				}
			}
		}
	}
	return closest;
}




//------------------------------------------------------Addons-------------------------------------------//


AddonTask::AddonTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority)
	:Task(taskID, pre, unitName, supplyCount, x, y, newPriority)
{
	addon = UnitTypes::getUnitType(unitName);
}

string AddonTask::getName()
{
	return addon.getName();
}

void AddonTask::execute(int &mins, int &gas)
{
	//costMins += addon.mineralPrice();
	//costGas += addon.gasPrice();
	if(Broodwar->canMake(NULL,addon) && mins >= addon.mineralPrice() && gas >= addon.gasPrice())
	{
		if(!has_Worker)
		{
			worker = getWorker();
			if(worker != NULL)
				has_Worker = true;
		}

		if(has_Worker) // If a new worker was found this task already had a worker assigned
		{
			if(!worker->isTraining())
			{
				worker->buildAddon(addon);
				//mins -= addon.mineralPrice();
				//gas -= addon.gasPrice();
			}
			else
			{
				has_Worker = false;
				worker = getWorker();
				if(worker != NULL)
					has_Worker = true;
			}
		}
		//if(Broodwar->canMake(NULL,addon))
		{
			mins -= addon.mineralPrice();
			gas -= addon.gasPrice();
		}
	}
	
}

void AddonTask::cancel()
{
	if(worker)
	{
		worker->cancelAddon();
	}
}

Task* AddonTask::makeNew()
{
	return new AddonTask(taskNum, precondition, getName(), preconditionAmount, xPos, yPos, priority);
}

int AddonTask::checkStatus()
{
	if(!has_Worker)
		return 0;
	if(worker)
	{
		if(worker->isConstructing() && worker->getBuildUnit())
		{
			if(worker->getBuildUnit()->exists())
			{
				buildUnit = worker->getBuildUnit();
				has_buildUnit = true;			
				placer->freeTiles(buildUnit->getTilePosition(),addon.tileWidth(),addon.tileHeight());
				return 1;
			}
		}
	}

	if(has_buildUnit)
	{
		if(buildUnit)
			if(buildUnit->getRemainingBuildTime() == 0)
				return 2;
	}

	return 0;
}

Unit* AddonTask::getWorker()
{
	Position target = Position(xPos,yPos);
	int distance = 9999999;
	Unit* closest = NULL;
	UnitType trainer = addon.whatBuilds().first;
	for(unsigned int c = 0; c < buildings.size(); c++)
	{
		if(buildings[c]->getType() == trainer && !buildings[c]->isTraining() && !buildings[c]->getAddon())
		{
			if(buildings[c]->getDistance(target) < distance)
			{
				distance = buildings[c]->getDistance(target);
				closest = buildings[c];
			}
		}
	}
	return closest;
}




//------------------------------------------------------Tech-------------------------------------------//



TechTask::TechTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority, bool isActive)
	:Task(taskID, pre, unitName, supplyCount, x, y, newPriority)
{
	tech = TechTypes::getTechType(unitName);
	active = isActive;
}

string TechTask::getName()
{
	return tech.getName();
}

void TechTask::execute(int &mins, int &gas)
{
	//costMins += tech.mineralPrice();
	//costGas += tech.gasPrice();
	if(Broodwar->canResearch(NULL,tech) && mins >= tech.mineralPrice() && gas >= tech.gasPrice())
	{
		if(!has_Worker)
		{
			worker = getWorker();
			if(worker != NULL)
				has_Worker = true;
		}
		
		if(has_Worker)
		{
			if(worker)
				if(worker->research(tech))
				{
					active = true;
					mins -= tech.mineralPrice();
					gas -= tech.gasPrice();
				}
		}
	}

}

void TechTask::cancel()
{
	if(worker)
		worker->cancelResearch();
}

Task* TechTask::makeNew()
{
	return new TechTask(taskNum, precondition, getName(), preconditionAmount, xPos, yPos, priority, active);
}

int TechTask::checkStatus()
{
	if(!has_Worker)
		return 0;
	if(active && worker->getRemainingResearchTime() > 0)
		return 1;
	if(active && worker->getRemainingResearchTime() == 0)
		return 2;
	return 0;
}

Unit* TechTask::getWorker()
{
	Unit* building = NULL;
	UnitType trainer = tech.whatResearches();
	for(unsigned int c = 0; c < buildings.size(); c++)
	{
		if(buildings[c]->getType() == trainer)
			building = buildings[c];
	}
	return building;
}




//------------------------------------------------------Upgrades-------------------------------------------//



UpgradeTask::UpgradeTask(int taskID, Precon pre, string unitName, int supplyCount, int x, int y, int newPriority, bool isActive)
	:Task(taskID, pre, unitName, supplyCount, x, y, newPriority)
{
	ups = UpgradeTypes::getUpgradeType(unitName);
	active = isActive;
}

string UpgradeTask::getName()
{
	return ups.getName();
}

void UpgradeTask::execute(int &mins, int &gas)
{
	//costMins += ups.mineralPrice();
	//costGas += ups.gasPrice();
	if(Broodwar->canUpgrade(NULL,ups) && mins >= ups.mineralPrice() && gas >= ups.gasPrice())
	{
		if(!has_Worker)
		{
			worker = getWorker();
			if(worker != NULL)
				has_Worker = true;
		}
		
		if(has_Worker)
		{
			if(worker)
				if(worker->upgrade(ups))
				{
					active = true;
					mins -= ups.mineralPrice();
					gas -= ups.gasPrice();
				}
		}
	}
	
}

void UpgradeTask::cancel()
{
	if(worker)
		worker->cancelUpgrade();
}

Task* UpgradeTask::makeNew()
{
	return new UpgradeTask(taskNum, precondition, getName(), preconditionAmount, xPos, yPos, priority, active);
}

int UpgradeTask::checkStatus()
{
	if(!has_Worker)
		return 0;
	if(active && worker->getRemainingUpgradeTime() > 0)
		return 1;
	if(active && worker->getRemainingUpgradeTime() == 0)
		return 2;
	return 0;
}

Unit* UpgradeTask::getWorker()
{
	Unit* building = NULL;
	UnitType trainer = ups.whatUpgrades();
	for(unsigned int c = 0; c < buildings.size(); c++)
	{
		if(buildings[c]->getType() == trainer)
			building = buildings[c];
	}
	return building;
}



//------------------------------------------------------Expansions-------------------------------------------//
ExpandTask::ExpandTask(int x, int y)
	:Task(0,SUPPLY,"expand",0,x,y,100)
{


}

void ExpandTask::execute(int &mins, int &gas)
{
	//Broodwar->printf("%d, %d",BWTA::getNearestBaseLocation(xPos/32, yPos/32)->getTilePosition().x(),BWTA::getNearestBaseLocation(xPos/32, yPos/32)->getTilePosition().y());
	if(!has_Worker)
	{
		worker = getWorker();
		if(worker != NULL && worker->exists())
			has_Worker = true;
	}
	if(has_Worker)
	{
		if(worker && worker->exists())
		{		
			worker->lift();
			if(worker->isLifted())
			{
				worker->move(BWTA::getNearestBaseLocation(xPos/32, yPos/32)->getPosition());
				worker->land(BWTA::getNearestBaseLocation(xPos/32, yPos/32)->getTilePosition());
				//worker->land(TilePosition(20,20));
			}
		}
	}
}

void ExpandTask::cancel()
{

}

Task* ExpandTask::makeNew()
{
	return new ExpandTask(xPos,yPos);
}

int ExpandTask::checkStatus()
{
	
	if(has_Worker)
	{
		if(worker != NULL && worker->exists())
		{
			if(worker->isLifted())
			{
				worker->land(BWTA::getNearestBaseLocation(xPos/32, yPos/32)->getTilePosition());

				return 1;
			}
			if(!worker->isLifted() && BWTA::getNearestBaseLocation(worker->getTilePosition())->getTilePosition() == worker->getTilePosition())
			{
				return 2;
			}
		}
		else
		{
			has_Worker = false;
		}
	}
	
	//if((*Broodwar->getUnitsOnTile(xPos/32,yPos/32).begin()) != NULL)
		//if((*Broodwar->getUnitsOnTile(xPos/32,yPos/32).begin())->getType().isResourceDepot() && (*Broodwar->getUnitsOnTile(xPos/32,yPos/32).begin())->getPlayer() == Broodwar->self())
			//return 2;

	return 0;
}

string ExpandTask::getName()
{
	return "expand";
}

Unit* ExpandTask::getWorker()
{
	for(unsigned int c = 0; c < buildings.size(); c++)
	{
		if(buildings[c]->getType().isResourceDepot())
		{
			if(BWTA::getNearestBaseLocation(buildings[c]->getTilePosition())->getTilePosition() != buildings[c]->getTilePosition())
				return buildings[c];
		}
	}
	return NULL;
}




//------------------------------------------------------Scouting-------------------------------------------//

ScoutTask::ScoutTask(EnemyRegion* place)
	:Task(0,SUPPLY,"scout",0,0,0,100)
{
	region = place;
	scanCooldown = 20;
	hostileRegion = false;
}

void ScoutTask::execute(int &mins, int &gas)
{
	Broodwar->drawCircleMap(region->getRegion()->getCenter().x(), region->getRegion()->getCenter().y(), 100, Colors::Orange);
	Broodwar->drawCircleMap(region->getScoutPosition().x(), region->getScoutPosition().y(), 50, Colors::Orange);
	if(!has_Worker)
	{
		worker = getWorker();
		if(worker != NULL)
		{
			if(!hostileRegion || worker->getType().isBuilding())
				has_Worker = true;
		}
	}
	if(has_Worker)
	{
		if(worker)
		{
			//Broodwar->printf("%s",worker->getType().getName().c_str());
			Broodwar->drawCircleMap(worker->getPosition().x(), worker->getPosition().y(), 50, Colors::Yellow);
			Position pos = region->getScoutPosition();
			if(worker->getType().isBuilding())
			{
				if(scanCooldown == 20)
					worker->useTech(TechTypes::Scanner_Sweep, pos);
				scanCooldown --;
				if(scanCooldown == 0)
					scanCooldown = 20;
			}
			else
			{
				//if(worker->getTargetPosition() != pos)
					//worker->stop();
				if(pos.isValid())
					worker->move(pos);
				else
					Broodwar->printf("LOL INVALID POSITION");
			}

			if(!worker->exists() || (worker->getType() == UnitTypes::Terran_Comsat_Station && worker->getEnergy() < 50) || worker->isConstructing())
			{
				if(!worker->exists())
					hostileRegion = true;
				worker = NULL;
				has_Worker = false;
			}
		}
	}
}

void ScoutTask::cancel()
{
	if(worker)
		worker->stop();
}

Task* ScoutTask::makeNew()
{
	return new ScoutTask(region);
}

int ScoutTask::checkStatus()
{
	return region->getPercentageScouted();
}

string ScoutTask::getName()
{
	return "SCOUTING!!!!!!";
}

Unit* ScoutTask::getWorker()
{
	for(unsigned int c = 0; c < buildings.size(); c++)
	{
		if(buildings[c]->getType() == UnitTypes::Terran_Comsat_Station && buildings[c]->getEnergy() > 50)
			return buildings[c];
	}

	Unit* closest = NULL;
	int dist = 99999999, tdist;
	set<Unit*> units = Broodwar->self()->getUnits();
	set<Unit*>::iterator i;
	for(i = units.begin(); i != units.end(); i++)
	{
		if((*i)->isIdle() && !(*i)->getType().isWorker() && !(*i)->getType().isBuilding())
		{
			tdist = region->getRegion()->getCenter().getApproxDistance((*i)->getPosition());
			if(tdist < dist)
			{
				dist = tdist;
				closest = (*i);
			}
			//return (*i);
		}
	}
	if(dist < 99999999)
		return closest;
	for(unsigned int c = 0; c < workers.size(); c++)
	{
		if(workers[c]->isGatheringMinerals())
		{
			tdist = region->getRegion()->getCenter().getApproxDistance(workers[c]->getPosition());
			if(tdist < dist)
			{
				dist = tdist;
				closest = workers[c];
			}
			//return workers[c];
		}
	}
	if(dist < 99999999)
		return closest;
	else
		return NULL;
}