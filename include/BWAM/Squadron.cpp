// TODO: Add other includes
#include <BWAM\Squadron.h>
#include <BWAM\ArmyTasks\ArmyTask.h>

// Maps each unit to the team to which they are assigned
std::map<BWAPI::Unit*, Fireteam*> Squadron::mapUnitToTeam;
std::map<Fireteam*,Squadron*> Squadron::mapTeamToSquad;

void Squadron::onUnitRemoved(BWAPI::Unit *unit, Fireteam *team)
{
	//BWAPI::Broodwar->printf("A unit has been removed!");

}

// STATIC
void Squadron::decommissionUnit(BWAPI::Unit* unit) {
	Fireteam *team = NULL; 
	Squadron *squad = NULL;

	if(mapUnitToTeam.find(unit) != mapUnitToTeam.end())
	{
		team = mapUnitToTeam[unit];
	}
	if (team) 
	{
		if(mapTeamToSquad.find(team) != mapTeamToSquad.end())
		{
			squad = mapTeamToSquad[team];
		}
		team->removeUnit(unit);
		mapUnitToTeam.erase(unit);
		if(squad && team->isDestroyed())
		{
			squad->removeTeam(team); // This will also call mapTeamToSquad.erase(team) and adjustTeams()
			squad->adjustTeams();
			//delete (team);
		}
		// TODO: remove unit from any and all other maps as well 
	}
}

// STATIC
void Squadron::clearNonexistentUnits()
{	
	// Temporary fix for !exists units
	std::queue<BWAPI::Unit*> nonExistentUnits;
	for(std::map<Fireteam*,Squadron*>::const_iterator team = mapTeamToSquad.begin(); team != mapTeamToSquad.end(); team++)
	{
		if( (*team).first )
		{
			for(std::set<BWAPI::Unit*>::const_iterator unit = (*team).first->units.begin(); unit != (*team).first->units.end(); unit++)
			{
				if ( !((*unit) && (*unit)->exists()) )
				{
					nonExistentUnits.push((*unit));
				}
			}
		}
	}

	while(nonExistentUnits.empty() == false)
	{
		decommissionUnit(nonExistentUnits.front());
		nonExistentUnits.pop();
	}
	// End temporary fix
}

Squadron::Squadron(){
	taskParameters.teamSet = &teams;
	currentTask = ArmyTask::taskMap["Idle"];
	taskParameters.taskFailed = false;
	taskParameters.taskComplete = false;
	taskParameters.taskInProgress = false;
}

void Squadron::adjustTeams()
{
	currentTask->adjustTeams(&taskParameters);
}

void Squadron::addTeam(Fireteam* team) 
{ 
	teams.insert(team);
	currentTask->adjustTeams(&taskParameters);
	mapTeamToSquad[team] = this;
}

void Squadron::removeTeam(Fireteam* team) 
{ 
	teams.erase(team);
	mapTeamToSquad.erase(team);
	adjustTeams();
} // Removes team from squadron 

void Squadron::transferTo(Squadron* target, BWAPI::UnitType type, int number)
{
	int leftToTransfer = number;
	std::queue<BWAPI::Unit*> transferQueue;
	for(std::set<Fireteam*>::const_iterator i=teams.begin(); i != teams.end() && leftToTransfer > 0; i++)
	{
		if ((*i)->getUnitType() == type) 
		{
			std::set<BWAPI::Unit*>::const_iterator u = (*i)->units.begin();		
			while(leftToTransfer > 0 && u != (*i)->units.end())
			{
				transferQueue.push((*u));
				u++;
				--leftToTransfer;
			}
		}	
	}
	while(transferQueue.empty() == false)
	{
		target->addUnit(transferQueue.front()); // This handles the removal from the previous team/squad
		transferQueue.pop();
	}
}// Transfers up to int number of UnitType units to Squadron*

void Squadron::mergeWith(Squadron *target)
{

}// Transfers all units to Squadron*, leaving this Squadron empty


void Squadron::addUnit(BWAPI::Unit *unit) 
{	
	if (! (unit && unit->exists() && unit->getPlayer() == BWAPI::Broodwar->self()))
	{
		// Not a unit that can be added to a Squadron
		return;
	}
	Fireteam *newTeam = Fireteam::createTeam(unit->getType());
	// TODO: Add a "validUnitType" function for this instead?
	if(!newTeam) // Fireteams cannot be created from this unit type
	{
		delete (newTeam);
		return;
	}
	
	Squadron::decommissionUnit(unit); //Remove from all previous teams and squadrons

	// Temporary? This does not allow multiple teams of one type
	if(!firstPartialTeam(unit->getType())) {
		addTeam(newTeam);
	} else {
		delete (newTeam);
	}
	mapUnitToTeam[unit] = firstPartialTeam(unit->getType());
	mapUnitToTeam[unit]->addUnit(unit);
}



int Squadron::getTotalOfType(BWAPI::UnitType typeToCount)
{
	int count = 0;

	for(std::set<Fireteam*>::const_iterator i=teams.begin(); i != teams.end(); i++)
	{
		if((*i)->getUnitType() == typeToCount)
		{
			count += (*i)->getUnitCount();
		}
	}
	return count;
}	// Returns the total number of units of a given type contained in this Squadron.


	// TODO: ? make this private?
Fireteam *Squadron::firstPartialTeam(BWAPI::UnitType type)	// Returns the first non-full Fireteam of the given type
{
	Fireteam *toReturn = NULL;

	for(std::set<Fireteam*>::const_iterator i=teams.begin(); i != teams.end(); i++)
	{
		if((*i)->getUnitType() == type)
		{
			return (*i);
		}
	}
	return toReturn;
}
	
bool Squadron::taskInProgress()
{
	currentTask->updateTaskProgress(&taskParameters);
	// Calculate conditions here
	return taskParameters.taskInProgress;
}

bool Squadron::taskComplete()
{
	currentTask->updateTaskProgress(&taskParameters);
	// Calculate conditions here
	return taskParameters.taskComplete;
}

bool Squadron::taskFailed()
{
	currentTask->updateTaskProgress(&taskParameters);
	return taskParameters.taskFailed;
}

std::string Squadron::getTaskName()
{
	return taskParameters.taskName;
}// Returns the name of the current task
	


void Squadron::task(std::string key) {
	
	currentTask = ArmyTask::taskMap[key];
	if(currentTask)
	{
		taskParameters.taskName = key;
		currentTask->adjustTeams(&taskParameters);
	}
	else
	{
		taskParameters.taskName = "Idle";
		currentTask = ArmyTask::taskMap["Idle"];
	}
	taskParameters.taskFailed = false;
	taskParameters.taskInProgress = false;
	taskParameters.taskComplete = false;
}	

void Squadron::doTask() {
	if(taskParameters.taskInProgress == false) {
		taskParameters.taskInProgress = true;
		currentTask->doTask(&taskParameters);
	}
}

void Squadron::updateTeamMicro()
{
	currentTask->updateTeamMicro(&taskParameters);
}

void Squadron::setObjectivePosition(BWAPI::Position pos) {
	taskParameters.objectivePosition = pos;
	
}		// defend | attack | etc etc  
