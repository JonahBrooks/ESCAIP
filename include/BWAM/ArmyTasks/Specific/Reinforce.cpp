#include <BWAM\ArmyTasks\Specific\SpecificArmyTasks.h>

void Reinforce::doTask(ArmyTaskContext *params) {
	if (params->objectivePosition) 
	{
		for(std::set<Fireteam*>::const_iterator i=params->attackTeams.begin(); i != params->attackTeams.end(); i++)
		{
			//BWAPI::Broodwar->printf("Attack team!");
			(*i)->deploy(params->objectivePosition);
			//BWAPI::Broodwar->printf("%d",(*i)->getUnitCount());
			BWAPI::Broodwar->printf("%s team deployed!",(*i)->getUnitType().c_str());
			//BWAPI::Broodwar->printf("Team has been deployed.");
			
		}
		for(std::set<Fireteam*>::const_iterator i=params->supportTeams.begin(); i != params->supportTeams.end(); i++)
		{
			if(!params->attackTeams.empty()) 
			{
				(*i)->assist(*(params->attackTeams.begin()));
//				BWAPI::Broodwar->printf("%s team assisting %s team!", 
	//					(*i)->getUnitType().c_str(), 
		//				(*params->attackTeams.begin())->getUnitType().c_str());	
			}
			else
			{
				(*i)->deploy(params->objectivePosition);
			}
		}
	}
}

void Reinforce::updateTaskProgress(ArmyTaskContext *params) 
{ 
	
}

void Reinforce::adjustTeams(ArmyTaskContext *params) {
	params->attackTeams.clear();
	params->supportTeams.clear();
	params->siegeTeams.clear();
	for(std::set<Fireteam*>::const_iterator i=params->teamSet->begin(); i != params->teamSet->end(); i++)
	{
		BWAPI::UnitType itype = (*i)->getUnitType();
		if((itype == BWAPI::UnitTypes::Terran_Medic) || (itype == BWAPI::UnitTypes::Terran_Science_Vessel))
		{
			params->supportTeams.insert((*i));
		}
		else if(itype == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode || itype == BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode)
		{
			params->siegeTeams.insert((*i));
		}
		else
		{
			params->attackTeams.insert((*i));
		}
	}
}

void Reinforce::updateTeamMicro(ArmyTaskContext *params)
{
	ArmyTask::updateTeamMicro(params);
}