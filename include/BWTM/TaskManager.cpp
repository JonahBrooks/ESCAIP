#include "TaskManager.h"

typedef pair<UnitType,int> unitPair;

TaskManager::TaskManager(char* fileName)
{
	activeUnits.insert(unitPair(UnitTypes::Terran_Marine,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Medic,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Ghost,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Firebat,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Vulture,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Siege_Tank_Tank_Mode,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Goliath,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Wraith,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Valkyrie,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Science_Vessel,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Dropship,0));
	activeUnits.insert(unitPair(UnitTypes::Terran_Battlecruiser,0));

	inactiveUnits.insert(unitPair(UnitTypes::Terran_Marine,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Medic,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Ghost,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Firebat,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Vulture,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Siege_Tank_Tank_Mode,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Goliath,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Wraith,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Valkyrie,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Science_Vessel,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Dropship,0));
	inactiveUnits.insert(unitPair(UnitTypes::Terran_Battlecruiser,0));

	active = new TaskList();
	inactive = new TaskList();
	waiting = new TaskList();
	completed = new TaskList();
	workerCount = 0;
	maxWorkers = 50;
	readFile(fileName);
}

void TaskManager::readFile(char* fileName)
{
	char cLine[256];
	string* line;

	ifstream input(fileName);
	if(input.is_open())
	{
		Broodwar->printf("Loading predefined build order from file.");
		while(input.good())
		{
			input.getline(cLine,256);
			line = new string(cLine);
			inactive->addTask(*line);
		}
		inactive->addMaps(&inactiveUnits);
	}

}

void TaskManager::addTask(string newTask)
{
	inactive->addTask(newTask);
}

void TaskManager::expand(int x, int y)
{
	inactive->addTask(new ExpandTask(x,y));
}

void TaskManager::addUnitTask(UnitType unit, int amount, int priority)
{
	stringstream temp(stringstream::in | stringstream::out);
	temp << "<" << unit.getName() << ", " << amount << ">";
	temp << " ";
	temp << "{" << priority << "}";
	//Broodwar->printf("%s",temp.str().c_str());
	inactive->addTask(temp.str());
	inactiveUnits[unit] += amount;
}

void TaskManager::addUnitTask(UnitType unit, TilePosition tPos, int priority)
{
	stringstream temp(stringstream::in | stringstream::out);
	temp << "<" << unit.getName() << ">";
	temp << " ";
	temp << "[" << tPos.x() * 32 << " " << tPos.y() * 32 << "]";
	temp << " ";
	temp << "{" << priority << "}";
	//Broodwar->printf("%s",temp.str().c_str());
	inactive->addTask(temp.str());
	inactiveUnits[unit] += 1;
}

void TaskManager::addTechTask(TechType tech, int priority)
{
	stringstream temp(stringstream::in | stringstream::out);
	temp << "<" << tech.getName() << ">";
	temp << " ";
	temp << "{" << priority << "}";
	//Broodwar->printf("%s",temp.str().c_str());
	inactive->addTask(temp.str());
}

void TaskManager::addUpgradeTask(UpgradeType ups, int priority)
{
	stringstream temp(stringstream::in | stringstream::out);
	temp << "<" << ups.getName() << ">";
	temp << " ";
	temp << "{" << priority << "}";
	//Broodwar->printf("%s",temp.str().c_str());
	inactive->addTask(temp.str());
}

void TaskManager::addExpansionTask(TilePosition tPos, int priority)
{
	addUnitTask(UnitTypes::Terran_Command_Center, 1, priority);
	expand(tPos.x() * 32, tPos.y() * 32);
}

int TaskManager::getActiveUnits(UnitType unit)
{
	return activeUnits[unit];
}

int TaskManager::getInactiveUnits(UnitType unit)
{
	return inactiveUnits[unit];
}

void TaskManager::clearFromActiveMap(UnitType unit, int amount)
{
	activeUnits[unit] -= amount;
}

void TaskManager::addUnit(Unit* newUnit)
{
	Task::addUnit(newUnit);
	if(newUnit->getPlayer() != Broodwar->self())
		return;
	if(newUnit->getType().isWorker())
	{
		workerCount ++;
		return;
	}
	if(newUnit->getType().isBuilding())
	{
		if(activeUnits[newUnit->getType()] > 0)
		{
			activeUnits[newUnit->getType()]--;
		}
		buildings.push_back(newUnit);
		return;
	}
}

bool TaskManager::prereqCheck(Task* task)
{
	switch(task->getPrecondition())
	{
		case(WORKERS):
			if(workerCount >= task->getSupply())
				return true;
			else
				return false;
			break;
		case(SECONDS):
			if(Broodwar->elapsedTime() >= task->getSupply())
				return true;
			else
				return false;
			break;
		case(TASK_FINISHED):
			if(completed->contains(task->getSupply()) != 0)
				return true;
			else
				return false;
			break;
		case(TASK_IN_PROGRESS):
			if(active->contains(task->getSupply()) != 0)
				return true;
			else
				return false;
			break;
		case(SUPPLY):
			if(Broodwar->self()->supplyUsed() >= (task->getSupply() * 2))
				return true;
			else
				return false;
			break;
		default:
			return false;
			break;
	}
}

void TaskManager::manageInactiveTasks()
{
	Task* newTask = inactive->getFront();
	for(int c = 0; c < inactive->getSize(); c++)
	{
		if(prereqCheck(newTask))
		{
			newTask = addToWaitingQueue(newTask);
		}
		newTask = newTask->next;
	}
}

Task* TaskManager::addToWaitingQueue(Task* newTask)
{
	int unitCount = 0, bldgCount = 0;
	UnitType unit = UnitTypes::getUnitType(newTask->getName());
	if(unit != UnitTypes::Unknown)
	{
		UnitType trainer = unit.whatBuilds().first;
		int size = buildings.size();
		for(int c = 0; c < size; c++)
			if(buildings[c]->getType() == trainer && !buildings[c]->isTraining())
				bldgCount++;
		
		unitCount = getNumWaiting(newTask->getName());
	}
	if(unitCount <= bldgCount)
	{
		if(unit == UnitTypes::Terran_SCV)
		{
			if(workerCount + unitCount > maxWorkers)
				return newTask;
		}
		waiting->addTask(newTask);
		newTask = inactive->removeTask(newTask);
	}
	return newTask;
}

int TaskManager::getNumWaiting(string unitName)
{
	int count = 0;
	Task* newTask = waiting->getFront();
	for(int c = 0; c < waiting->getSize(); c++)
	{
		if(strcmp(newTask->getName().c_str(),unitName.c_str()) == 0)
		{
			count ++;
		}
		newTask = newTask->next;
	}
	return count;
}

void TaskManager::manageWaitingTasks()
{
	string lol = "";
	int reserveMins = Broodwar->self()->minerals(), reserveGas = Broodwar->self()->gas();
	Task* newTask = waiting->getFront();

	for(int c = 0; c < waiting->getSize(); c++)
	{
		newTask->execute(reserveMins, reserveGas);
		lol += newTask->getName();
		lol += " ";

		if(newTask->checkStatus() != 0)
		{
			if(inactiveUnits[UnitTypes::getUnitType(newTask->getName())] > 0)
			{
				inactiveUnits[UnitTypes::getUnitType(newTask->getName())] -= 1;
				activeUnits[UnitTypes::getUnitType(newTask->getName())] += 1;
			}
			active->addTask(newTask);
			newTask = waiting->removeTask(newTask);
			
		}

		if(reserveMins < 0)
			reserveMins = 0;
		if(reserveGas < 0)
			reserveGas = 0;

		newTask = newTask->next;
	} // end for loop

	//Broodwar->printf("%s",lol.c_str());
}

void TaskManager::manageActiveTasks()
{
	Task* newTask = active->getFront();
	for(int c = 0; c < active->getSize(); c++)
	{
		if(newTask->checkStatus() == 2)
		{
			/*UnitType temp = UnitTypes::getUnitType(newTask->getName());
			if(temp != UnitTypes::Unknown)
			{
				activeUnits[temp] --;
			}*/
			completed->addTask(newTask);
			newTask = active->removeTask(newTask);
			//Broodwar->printf("TASK COMPLETED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		}
		newTask = newTask->next;
	}
}

void TaskManager::manageTasks()
{
	//Broodwar->printf("inactive:%d, waiting:%d, active:%d, completed:%d",inactive->getSize(),waiting->getSize(),active->getSize(),completed->getSize());
	//Broodwar->printf("Supply Depots inactive: %d", inactiveUnits[UnitTypes::Terran_Supply_Depot]);

	set<UnitType> wait;
	getTasksFromList(wait, inactive);
	if(wait.find(UnitTypes::Terran_SCV) == wait.end())
		addUnitTask(UnitTypes::Terran_SCV,1,workerPriority);

	if(!inactive->isEmpty())
	{
		manageInactiveTasks();
	}

	if(!active->isEmpty())
	{
		manageActiveTasks();
	}
	
	if(!waiting->isEmpty())
	{
		manageWaitingTasks();
	}

}

void TaskManager::checkDestroyed(Unit* destroyedUnit)
{
	Task::checkDestroyed(destroyedUnit);
	if(destroyedUnit->getPlayer() == Broodwar->self() && destroyedUnit->getType().isWorker())
		workerCount --;
			
	if(destroyedUnit->getPlayer() == Broodwar->self() && destroyedUnit->getType().isBuilding())
	{
		for(unsigned int c = 0; c < buildings.size(); c++)
			if(buildings[c] == destroyedUnit)
			{
				buildings.erase(buildings.begin() + c);
			}
	}
}

void TaskManager::scrapTask(UnitType unit)
{
	scrapTaskFromList(unit, inactive);
	scrapTaskFromList(unit, waiting);
	scrapTaskFromList(unit, active);
}

void TaskManager::scrapTaskFromList(UnitType unit, TaskList* list)
{
	Task* newTask = list->getFront()->prev;
	for(int c = 0; c < list->getSize(); c++)
	{
		if(UnitTypes::getUnitType(newTask->getName()) == unit)
		{
			newTask->cancel();
			newTask = list->removeTask(newTask);
		}
		newTask = newTask->next;
	}
}

set<UnitType> TaskManager::getPendingTasks()
{
	set<UnitType> tasks;
	getTasksFromList(tasks, inactive);
	getTasksFromList(tasks, waiting);
	getTasksFromList(tasks, active);
	return tasks;
}

set<UnitType> TaskManager::getCompletedTasks()
{
	set<UnitType> tasks;
	getTasksFromList(tasks, completed);
	return tasks;
}

void TaskManager::getTasksFromList(set<UnitType> &tasks, TaskList *list)
{
	Task* newTask = list->getFront();
	for(int c = 0; c < list->getSize(); c++)
	{
		if(UnitTypes::getUnitType(newTask->getName()) != UnitTypes::Unknown)
			tasks.insert(UnitTypes::getUnitType(newTask->getName()));
		newTask = newTask->next;
	}
}

void TaskManager::setWorkerMax(int newMax)
{
	maxWorkers = newMax;
}

bool TaskManager::expandTaskExists()
{
	Task* newTask = waiting->getFront();
	for(int c = 0; c < waiting->getSize(); c++)
	{
		if(strcmp(newTask->getName().c_str(),"expand") == 0)
			return true;
		newTask = newTask->next;
	}
	newTask = active->getFront();
	for(int c = 0; c < active->getSize(); c++)
	{
		if(strcmp(newTask->getName().c_str(),"expand") == 0)
			return true;
		newTask = newTask->next;
	}
	return false;
}

TaskManager::~TaskManager(void)
{
	delete inactive;
	delete active;
	delete waiting;
	delete completed;
}
