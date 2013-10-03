#include "Refinery.h"


Refinery::Refinery(Unit* newRefinery)
{
	refinery = newRefinery;
	maxCollectors = 3;
	gatherers.push_back(refinery->getBuildUnit());
	/*set<Unit*> units = Broodwar->self()->getUnits();
	set<Unit*>::iterator c;
	for(c = units.begin(); c != units.end(); c++)
	{
		if((*c)->getType().isWorker() && (*c)->exists())
		{
			if((*c)->isGatheringGas() && (*c)->getDistance(refinery->getPosition()) < 100)
			{
				gatherers.push_back(*c);
			}
		}
	}*/
}


Refinery::~Refinery(void)
{
}

void Refinery::grabWorker()
{
	set<Unit*> units = Broodwar->self()->getUnits();
	for(set<Unit*>::iterator c = units.begin(); c != units.end(); c++)
	{
		if((*c)->exists() && (*c)->getType().isWorker() && (*c)->getDistance(refinery) < 200 && (*c)->isGatheringMinerals() && !(*c)->isCarryingMinerals())
		{
			(*c)->rightClick(refinery);
			gatherers.push_back(*c);
			break;
		}
	}

}
void Refinery::manageRefinery()
{
	if(refinery->getRemainingBuildTime() == 0)
	{
		for(unsigned int c = 0; c < gatherers.size(); c++)
			{
				if(gatherers[c] && (!gatherers[c]->isGatheringGas() || !gatherers[c]->exists()))
				{
					gatherers.erase(gatherers.begin()+c);
				}
			}

		if(gatherers.size() < (unsigned int)maxCollectors)
		{
			grabWorker();
		}

		if(gatherers.size() > (unsigned int)maxCollectors)
		{
			if(gatherers[0])
			{
				gatherers[0]->stop();
			}
		}
	}
}
void Refinery::setMax(int newMax)
{
	maxCollectors = newMax;
}
bool Refinery::inExistence()
{
	bool exist = true;
	if(!refinery)
		return false;
	if(!refinery->exists())
		exist = false;
	if(refinery->getPlayer() != Broodwar->self())
		exist = false;
	return exist;
}
bool Refinery::isSat()
{
	if(maxCollectors >= 3)
		return true;
	return false;
}