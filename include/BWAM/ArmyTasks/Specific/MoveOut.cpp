#include <BWAM\ArmyTasks\Specific\SpecificArmyTasks.h>

//***************************************************************************************
//** MoveOut is designed to move an entire Squadron to a desired location				*
//** Units of all types should move together, and only engage enemies in line of sight  *
//***************************************************************************************

void MoveOut::doTask(ArmyTaskContext *params) {
	if (params->objectivePosition) 
	{
		//BWAPI::Broodwar->printf("Moving out!");
		for(std::set<Fireteam*>::const_iterator i=params->attackTeams.begin(); i != params->attackTeams.end(); i++)
		{
			//(*i)->moveFormation(params->objectivePosition);
			(*i)->deploy(params->objectivePosition);
		}
		for(std::set<Fireteam*>::const_iterator i=params->siegeTeams.begin(); i != params->siegeTeams.end(); i++)
		{
			(*i)->deploy(params->objectivePosition);
		}
		for(std::set<Fireteam*>::const_iterator i=params->supportTeams.begin(); i != params->supportTeams.end(); i++)
		{
			if(!params->attackTeams.empty()) 
			{
				(*i)->assist(*(params->attackTeams.begin()));
			}
			else if(!params->siegeTeams.empty())
			{
				(*i)->assist(*(params->siegeTeams.begin()));
			}
			else
			{
				(*i)->deploy(params->objectivePosition);
			}
		}
	}
}

void MoveOut::updateTaskProgress(ArmyTaskContext *params) 
{ 
	bool squadDestroyed = true;

	for(std::set<Fireteam*>::const_iterator i=params->teamSet->begin(); i != params->teamSet->end(); i++)
	{
		if((*i) && (*i)->getUnitCount() > 0)
		{	
			if((*i)->getUnitType() != BWAPI::UnitTypes::Terran_Medic && (*i)->getUnitType() != BWAPI::UnitTypes::Terran_Science_Vessel)
			{
				squadDestroyed = false;
			}
			int x1 = (*i)->getPosition().x();
			int x2 = params->objectivePosition.x();
			int y1 = (*i)->getPosition().y();
			int y2 = params->objectivePosition.y();

			if(abs(x1 - x2) < 300 && abs(y1 - y2) < 300) 
			{
				if(!params->taskComplete)
				{
					//BWAPI::Broodwar->printf("MoveOut Complete!");
				}
				params->taskComplete = true;
				params->taskInProgress = false;
			}
		}
	}
	if(squadDestroyed)
	{
		if(!params->taskFailed)
		{
			//BWAPI::Broodwar->printf("MoveOut Failed!");
		}
		params->taskFailed = true;
		params->taskComplete = false;
		params->taskInProgress = false;
	}
}

void MoveOut::adjustTeams(ArmyTaskContext *params) {
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

void MoveOut::updateTeamMicro(ArmyTaskContext *params)
{
	//ArmyTask::updateTeamMicro(params);
	for(std::set<Fireteam*>::const_iterator i=params->attackTeams.begin(); i != params->attackTeams.end(); i++)
	{
		if(!(*i)->useAbilityOnAttack("Stim_Packs", false))
		{
			if((*i)->isInCombat() == false)
			{
				(*i)->deploy(params->objectivePosition);
			}
				//doTask(params);
		}
	}
	for(std::set<Fireteam*>::const_iterator i=params->siegeTeams.begin(); i != params->siegeTeams.end(); i++)
	{
		if(	!(*i)->useAbilityOnSight("Tank_Siege_Mode", BWAPI::Broodwar->self()->weaponMaxRange(BWAPI::WeaponTypes::Arclite_Shock_Cannon), true, false) &&
			!(*i)->useAbilityOnClear("Unsiege", BWAPI::Broodwar->self()->weaponMaxRange(BWAPI::WeaponTypes::Arclite_Shock_Cannon), true, false))
		{ 
			(*i)->deploy(params->objectivePosition);
			//doTask(params);
		}
	}

	for(std::set<Fireteam*>::const_iterator i=params->supportTeams.begin(); i != params->supportTeams.end(); i++)
	{
		if(!params->attackTeams.empty()) 
		{
			(*i)->assist(*(params->attackTeams.begin()));
		}
	}
}
