#include "TaskList.h"


TaskList::TaskList(void)
{
	Front = new BuildingTask(-1, WORKERS, "sentinel",999,-1,-1,false);
	Front->next = Front;
	Front->prev = Front;
	size = 0;
}

void TaskList::addUnitToMap(UnitType unit)
{
	unitMap[unit] ++;
}

void TaskList::addMaps(map<UnitType,int> *toMap)
{
	map<UnitType,int>::iterator c;
	for(c = unitMap.begin(); c != unitMap.end(); c++)
	{
		(*toMap)[(*c).first] += unitMap[(*c).first];
	}
}

void TaskList::addTask(Task* newTask)
{
	Task* temp;

	temp = newTask->makeNew();

	if(temp)
	{
		if(newTask->hasWorker())
			temp->setWorker(newTask->getWorker());
		if(newTask->hasBuildUnit())
			temp->setBuildUnit(newTask->getBuildUnit());
		if(newTask->isWall())
			temp->setWall();
	}

	if(this->isEmpty())
	{
		Front->prev->next = temp;
		temp->prev = Front->prev;
		temp->next = Front;
		Front->prev = temp;
	}
	else
	{
		Task* behindThis = Front->prev;
		while(behindThis != Front && temp->getPriority() > behindThis->getPriority())
		{
			behindThis = behindThis->prev;
		}
		behindThis->next->prev = temp;
		temp->next = behindThis->next;
		temp->prev = behindThis;
		behindThis->next = temp;
	}

	size ++;
}

void TaskList::addTask(string taskLine)
{
	string preconditions, unit;
	int taskP, taskL, taskN = 0, x = -1, y = -1, prePos, unitPos, endUnitPos, commaPos, xPos, yPos, prePosL, unitPosL, xPosL, yPosL, amount = 1, priority = 100;
	bool noPre = false, wall = false;

	if(taskLine.find('(') != string::npos)
	{
		prePos = taskLine.find('(') + 1;
		prePosL = taskLine.find(')') - prePos;
	}
	else
	{
		noPre = true;
	}

	if(taskLine.find('{') != string::npos)
	{
		int priPos = taskLine.find('{') + 1;
		int priPosL = taskLine.find('}') - priPos;
		priority = atoi(taskLine.substr(priPos,priPosL).c_str());
	}

	unitPos = taskLine.find('<') + 1;
	endUnitPos = taskLine.find('>');
	commaPos = taskLine.find(',',unitPos);
	if(commaPos != string::npos && commaPos < endUnitPos)
	{
		int amtLen = endUnitPos - commaPos - 1;
		endUnitPos = commaPos;
		amount = atoi(taskLine.substr(commaPos + 1, amtLen).c_str());
	}

	unitPosL = endUnitPos - unitPos;

	if(taskLine.find('[') != string::npos)
	{
		xPos = taskLine.find('[') + 1;

		for(unsigned int c = xPos; c < taskLine.length(); c++)
			if(taskLine[c] == ',')
				taskLine[c] = ' ';

		xPosL = taskLine.find(' ',xPos) - xPos;
		yPos = taskLine.find(' ',xPos) + 1;
		yPosL = taskLine.find(']') - yPos;
		x = atoi(taskLine.substr(xPos,xPosL).c_str());
		y = atoi(taskLine.substr(yPos,yPosL).c_str());
	}

	if(taskLine.find('#') != string::npos)
	{
		taskP = taskLine.find('#') + 1;
		taskL = taskLine.find(' ', taskP) - taskP; 
		taskN = atoi(taskLine.substr(taskP,taskL).c_str());
	}

	unit = taskLine.substr(unitPos, unitPosL);
	
	Precon pre;
	int preAmnt;
	if(!noPre)
	{
		preconditions = taskLine.substr(prePos, prePosL);
		int colonPos = preconditions.find(':');
		preAmnt = atoi(preconditions.substr(0,colonPos).c_str());
		char preFlag = preconditions[colonPos+1];
		
		switch(preFlag)
		{
			case 'w':
				pre = WORKERS; break;
			case 't':
				pre = SECONDS; break;
			case 'f':
				pre = TASK_FINISHED; break;
			case 'p':
				pre = TASK_IN_PROGRESS; break;
			case 's':
				pre = SUPPLY; break;
			default:
				pre = WORKERS; break;
		}
	}else
	{
		pre = WORKERS;
		preAmnt = 0;
	}

	if(taskLine.find("wall") != string::npos)
	{
		wall = true;
	}

	Task* temp = NULL;

	if(UnitTypes::getUnitType(unit) != UnitTypes::Unknown)
	{
		if(UnitTypes::getUnitType(unit).isBuilding())
		{
			if(UnitTypes::getUnitType(unit).isAddon())
				temp = new AddonTask(taskN, pre, unit, preAmnt, x, y, priority);
			else
				temp = new BuildingTask(taskN, pre, unit, preAmnt, x, y, priority);	
		}
		else
			temp = new UnitTask(taskN, pre, unit, preAmnt, x, y, priority, false);
		if(wall)
			temp->setWall();
		for(int c = 0; c < amount; c++)
			addTask(temp);
		addUnitToMap(UnitTypes::getUnitType(unit));
	}
	if(TechTypes::getTechType(unit) != TechTypes::Unknown)
	{
		temp = new TechTask(taskN, pre, unit, preAmnt, x, y, priority, false);
		if(wall)
			temp->setWall();
		for(int c = 0; c < amount; c++)
			addTask(temp);
	}
	if(UpgradeTypes::getUpgradeType(unit) != UpgradeTypes::Unknown)
	{
		temp = new UpgradeTask(taskN, pre, unit, preAmnt, x, y, priority, false);
		if(wall)
			temp->setWall();
		for(int c = 0; c < amount; c++)
			addTask(temp);
	}

	//Broodwar->printf("%d, %s, %d", taskN, unit.c_str(), preAmnt);

	if(temp)
		delete temp;
}

Task* TaskList::removeTask(Task* oldTask)
{
	Task* temp = oldTask->prev;
	oldTask->prev->next = oldTask->next;
	oldTask->next->prev = oldTask->prev;
	delete oldTask;
	size--;
	return temp;
}

Task* TaskList::removeFront()
{
	if(size > 0)
	{
		Task* temp = Front->next;
		Front->next = temp->next;
		temp->next->prev = Front;
		size--;
		return temp;
	}
	return 0;
}

Task* TaskList::contains(int taskID)
{
	Task* temp = Front->next;
	while(temp != Front)
	{
		if(temp->getTaskNum() == taskID)
			return temp;
		temp = temp->next;
	}
	return 0;
}

bool TaskList::isEmpty()
{
	if(size == 0)
		return true;
	return false;
}

Task* TaskList::getFront()
{
	return Front->next;
}

int TaskList::getSize()
{
	return size;
}

TaskList::~TaskList(void)
{
	delete Front;
}