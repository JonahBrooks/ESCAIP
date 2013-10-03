#include <BWAM\ArmyTasks\Specific\SpecificArmyTasks.h>

void Idle::doTask(ArmyTaskContext *params) { 
	// Moves all units to the idle point for this base
	if(params->objectivePosition.x() > 0 && params->objectivePosition.y() > 0)
	{
		for(std::set<Fireteam*>::iterator i = params->teamSet->begin(); i != params->teamSet->end(); i++)
		{
			(*i)->deploy(params->objectivePosition);
		}
	}
}
void Idle::updateTaskProgress(ArmyTaskContext *params) { 	
	params->taskInProgress = false;
	params->taskComplete = false;
}
void Idle::adjustTeams(ArmyTaskContext *params) {
}
void Idle::updateTeamMicro(ArmyTaskContext *params)
{
	ArmyTask::updateTeamMicro(params);
}