#include <BWAM\ArmyTasks\ArmyTask.h>

std::map<std::string,ArmyTask*> ArmyTask::taskMap;

void ArmyTask::updateTaskFailed(ArmyTaskContext *params) 
{
	bool squadDestroyed = true;

	for(std::set<Fireteam*>::const_iterator i=params->teamSet->begin(); i != params->teamSet->end(); i++)
	{
		if((*i) && (*i)->getUnitCount() > 0)
		{	
			if(   (*i)->getUnitType() != BWAPI::UnitTypes::Terran_Medic 
			   && (*i)->getUnitType() != BWAPI::UnitTypes::Terran_Science_Vessel
			   && (*i)->getUnitType() != BWAPI::UnitTypes::Terran_Dropship)
			{
				squadDestroyed = false;
			}
		}
	}
	if(squadDestroyed)
	{
		params->taskFailed = true;
		params->taskInProgress = false;
		params->taskComplete = false;
	}
}

void ArmyTask::updateTaskInProgress(ArmyTaskContext *params) 
{
	if(params->taskFailed || params->taskComplete)
	{
		params->taskInProgress = false;
	}
}

void ArmyTask::updateTaskComplete(ArmyTaskContext *params) 
{ 
}

void ArmyTask::updateTaskProgress(ArmyTaskContext *params) 
{
	updateTaskComplete(params);
	updateTaskFailed(params);
	updateTaskInProgress(params);
}
		

void ArmyTask::updateTeamMicro(ArmyTaskContext *params)
{
	for(std::set<Fireteam*>::const_iterator i = params->teamSet->begin(); i != params->teamSet->end(); i++)
	{	
		(*i)->updateMicro();
	}
}