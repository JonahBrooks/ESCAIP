/*

#include <BWAM\Fireteams\Specific\SpecificFireteams.h>

void Terran_Siege_Tank_Fireteam::deploy(BWAPI::Position pos) {
	//BWAPI::Broodwar->printf("Setting objectivePosition to coordinates (%d,%d)", pos.x(), pos.y());
	mission.objectivePosition = pos;
	if (mission.status == 0) {
		//BWAPI::Broodwar->printf("Warning! Deploying team before critical mass! (%ss)", coreType.c_str());
	} else {
		mission.status = 2;
	}
	//int j = 0;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		(*i)->attack(pos, 0);
	//	j++;	
	//	BWAPI::Broodwar->printf("Unit %d of %d has been deployed.", j, unitCount);
	}

}// Should be conditional on coreType


void Terran_Siege_Tank_Fireteam::assist(Fireteam* toAssist) { // Should be conditional on coreType and toAssist.getUnitType()
	if(toAssist->getUnitCount() == 0)
	{
		return;
	}

	std::set<BWAPI::Unit*>::const_iterator target = toAssist->units.begin();

	for(std::set<BWAPI::Unit*>::const_iterator i = units.begin(); i != units.end(); i++)
	{
		(*i)->follow((*target));
		if((++target) == toAssist->units.end())
		{
			target = toAssist->units.begin();
		}
	}
}

void Terran_Siege_Tank_Fireteam::updateMicro() 
{
	//Fireteam::updateMicro();
}

*/