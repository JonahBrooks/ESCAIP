#include "ScoutTaskManager.h"


ScoutTaskManager::ScoutTaskManager(int startingScout, int scoutInterval)
{
	started = false;
	set<BaseLocation*> bases = getStartLocations();
	BWTA::Region* reg = NULL;
	for(set<BaseLocation*>::iterator c = bases.begin(); c != bases.end(); c++)
	{
		if(Broodwar->getUnitsOnTile((*c)->getTilePosition().x(), (*c)->getTilePosition().y()).empty())
			reg = getRegion((*c)->getTilePosition());
	}
	enemy = new EnemyInfo(reg);
	current = new TaskList();
	completed = new TaskList();

	start = startingScout;
	interval = scoutInterval;
	frameCount = 0;
}

void ScoutTaskManager::manageTasks()
{
	Task* newTask = current->getFront();
	for(int c = 0; c < current->getSize(); c++)
	{
		newTask->execute(c,c);
		//Broodwar->printf("executed a scout task");
		if(newTask->checkStatus() > 70) //whatever condition means the area has been scouted sufficiently
		{
			completed->addTask(newTask);
			newTask = current->removeTask(newTask);
		}
		newTask = newTask->next;
	}
}








void ScoutTaskManager::addTask(EnemyRegion* place)
{
	//Broodwar->printf("ADDING SCOUT TASK");
	Task* newTask = new ScoutTask(place);
	current->addTask(newTask);
}

void ScoutTaskManager::addTask(BWTA::Region* place)
{
	 vector<EnemyRegion*> regs = enemy->getRegions();
	 for(unsigned int c = 0; c < regs.size(); c++)
	 {
		 if(place == regs[c]->getRegion())
			 addTask(regs[c]);
	 }
}

void ScoutTaskManager::onFrame()
{
	enemy->onFrame();
	vector<EnemyRegion*> temp = enemy->getRegions();

	frameCount++;
	if(frameCount % 5 == 0)
		manageTasks();
	if(started && frameCount >= interval)
	{
		enemy->resetScouting();
		Task* loltask = current->getFront();
		for(int c = 0; c < current->getSize(); c++)
		{
			loltask->cancel();
			loltask = current->removeTask(loltask);
			loltask = loltask->next;
		}


		frameCount = 0;
		for(unsigned int c = 0; c < temp.size(); c++)
		{
			if(temp[c]->framesSinceReset() > 10000 || temp[c]->getMins() > 0 || temp[c]->buildingCount(UnitTypes::Terran_Command_Center) > 0 || temp[c]->buildingCount(UnitTypes::Protoss_Nexus) > 0 || temp[c]->buildingCount(UnitTypes::Zerg_Hatchery) > 0)
				addTask(temp[c]);
			//Broodwar->printf("Adding task at x position: %d", enemy->getRegions()[c]->getRegion()->getCenter());
		}
	}
	if(!started && frameCount >= start)
	{
		started = true;
		frameCount = 0;
		enemy->resetScouting();
		for(unsigned int c = 0; c < temp.size(); c++)
		{
			if(c == 0)
				addTask(temp[c]);
		}
	}

	//Debug Info
	stringstream tempstr(stringstream::in | stringstream::out);
	tempstr << "Tasks:          " << current->getSize() << endl
		<< "Regions:        " << enemy->getRegions().size() << endl
		<< "Missing Units:  " << enemy->getMissingUnits().size() << endl
		<< "Mineral Income: " << enemy->getMins() << endl
		<< "Gas Income:     " << enemy->getGas() << endl
		<< "% Main Scouted  " << enemy->getRegions()[0]->getPercentageScouted() << endl
		<< "--Units, Amount, Time First Seen" << endl;
	map<UnitType, int> disc = enemy->getDiscoveryTimes();
	map<UnitType, int> amt = enemy->getBuildingAmounts();
	for(map<UnitType, int>::iterator c = disc.begin(); c != disc.end(); c++)
	{
		tempstr << (*c).first.getName() << " " << amt[c->first] << " " << (*c).second << endl;
	}
	Broodwar->drawTextScreen(300,0,tempstr.str().c_str());
}

void ScoutTaskManager::onUnitDiscover(Unit* unit)
{
	enemy->onUnitDiscover(unit);
}

void ScoutTaskManager::onUnitDestroy(Unit* unit)
{
	enemy->onUnitDestroy(unit);
}

void ScoutTaskManager::onUnitEvade(Unit* unit)
{
	enemy->onUnitEvade(unit);
}

EnemyInfo* ScoutTaskManager::getEnemy()
{
	return enemy;
}

ScoutTaskManager::~ScoutTaskManager(void)
{
}
