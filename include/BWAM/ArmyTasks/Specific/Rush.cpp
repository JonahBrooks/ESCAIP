#include <BWAM\ArmyTasks\Specific\SpecificArmyTasks.h>

//***************************************************************************************
//** Rush is designed to send an entire Squadron to attack a given location				*
//** All units will rush to the given location and attack, regardless of pace.			*
//** Medics and SCVs, however, will follow units which they can support.				*
//***************************************************************************************

void Rush::doTask(ArmyTaskContext *params) {
	if (params->objectivePosition) 
	{
		for(std::set<Fireteam*>::const_iterator i=params->attackTeams.begin(); i != params->attackTeams.end(); i++)
		{
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
		}
	}
}


void Rush::updateTaskProgress(ArmyTaskContext *params) 
{ 
	bool squadDestroyed = true;

	for(std::set<Fireteam*>::const_iterator i=params->teamSet->begin(); i != params->teamSet->end(); i++)
	{
		if((*i) && (*i)->getUnitCount() > 0 && (*i)->getUnitType() != BWAPI::UnitTypes::Terran_Medic)
		{	
			squadDestroyed = false;
		}
	}
	if(squadDestroyed)
	{
		params->taskFailed = true;
		params->taskComplete = false;
		params->taskInProgress = false;
	}

	
}

void Rush::adjustTeams(ArmyTaskContext *params) {
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

void Rush::updateTeamMicro(ArmyTaskContext *params)
{
	//ArmyTask::updateTeamMicro(params);
	for(std::set<Fireteam*>::const_iterator i=params->attackTeams.begin(); i != params->attackTeams.end(); i++)
	{
		if(!(*i)->useAbilityOnAttack("Stim_Packs", false))
		{
			(*i)->deploy(params->objectivePosition);
		}
	}
	for(std::set<Fireteam*>::const_iterator i=params->siegeTeams.begin(); i != params->siegeTeams.end(); i++)
	{
		if(	!(*i)->useAbilityOnSight("Tank_Siege_Mode", BWAPI::Broodwar->self()->weaponMaxRange(BWAPI::WeaponTypes::Arclite_Shock_Cannon), true, false) &&
			!(*i)->useAbilityOnClear("Unsiege", BWAPI::Broodwar->self()->weaponMaxRange(BWAPI::WeaponTypes::Arclite_Shock_Cannon), true, false))
		{ 
			(*i)->deploy(params->objectivePosition);
		}
	}
}
