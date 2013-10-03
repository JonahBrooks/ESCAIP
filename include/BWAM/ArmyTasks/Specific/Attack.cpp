#include <BWAM\ArmyTasks\Specific\SpecificArmyTasks.h>

//***************************************************************************************
//** Attack is designed to send an entire Squadron to attack a given location			*
//** The units will destroy enemy units and structures near the target location			*
//** Once combat has ended the units will spiral outward until all enemies are destroyed*
//***************************************************************************************

void Attack::doTask(ArmyTaskContext *params) {

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


void Attack::updateTaskProgress(ArmyTaskContext *params) 
{ 
	bool squadDestroyed = true;
	bool inPosition = true;
	bool allClear = true;
	std::set<Fireteam*>::const_iterator teamIterator = params->teamSet->begin();
	while (teamIterator != params->teamSet->end())
	{
		if( (*teamIterator)->getUnitCount() > 0 
			&& (*teamIterator)->getUnitType() != BWAPI::UnitTypes::Terran_Medic 
			&& (*teamIterator)->getUnitType() != BWAPI::UnitTypes::Terran_Science_Vessel)
		{	
			squadDestroyed = false;
			if( abs((*teamIterator)->getPosition().x() - params->objectivePosition.x()) >= 300 && 
				abs((*teamIterator)->getPosition().y() - params->objectivePosition.y()) >= 300 ) 
			{
				inPosition = false; // This team is not in position
			}
		}
		
		teamIterator++; // Just increment the iterator
	}
	std::set<BWAPI::Unit*> unitsInRange = BWAPI::Broodwar->getUnitsInRadius(params->objectivePosition,600);
	for(std::set<BWAPI::Unit*>::const_iterator j=unitsInRange.begin(); 
		j != unitsInRange.end() && allClear; 
		j++)
	{
		if((*j) && (*j)->exists() && (*j)->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
		{
			allClear = false;		
		}
		
	}

	if(squadDestroyed)
	{
		params->taskFailed = true;
		params->taskComplete = false;
		params->taskInProgress = false;
	}
	else if(allClear && inPosition) // If all teams are at the objective position and no enemies remain
	{
		params->taskFailed = false;
		params->taskComplete = true;
		params->taskInProgress = false;
	}
	
}

void Attack::adjustTeams(ArmyTaskContext *params) {
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

void Attack::updateTeamMicro(ArmyTaskContext *params)
{
	updateTaskProgress(params); // ensures that there are no empty fireteams
	if (params->objectivePosition)
	{
		
		for(std::set<Fireteam*>::const_iterator i=params->teamSet->begin(); i != params->teamSet->end(); i++)
		{
			if( //(*i)->isInCombat() == false && 
				 abs((*i)->getPosition().x() - params->objectivePosition.x()) < 300 && 
				 abs((*i)->getPosition().y() - params->objectivePosition.y()) < 300 ) 
			{
				std::set<BWAPI::Unit*> unitsInRange = BWAPI::Broodwar->getUnitsInRadius(params->objectivePosition,600);
				bool enemyFound = false;
				for(std::set<BWAPI::Unit*>::const_iterator j=unitsInRange.begin(); 
					j != unitsInRange.end() && !enemyFound; 
					j++)
				{
					if((*j) && (*j)->exists())
					{
						if((*j)->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
						{
							//TODO: Check to see if the enemy is in this region
							(*i)->deploy((*j)->getPosition());
							//BWAPI::Broodwar->printf("Found an enemy in the region. Attack!!!");
							//TODO: Possibly update param objectivePosition?
							enemyFound = true;
						}
					}
					else
					{
						BWAPI::Broodwar->printf("J doesn't exists anymore ;_;");
					}
				}
				if (!enemyFound) {
					(*i)->deploy(params->objectivePosition);
				}
			}
			else
			{
				(*i)->deploy(params->objectivePosition);	
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
	//ArmyTask::updateTeamMicro(params);
	/*
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
	*/
}
