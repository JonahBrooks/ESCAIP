#include "MiningBase.h"

vector<Unit*>* MiningBase::workers = NULL;

MiningBase::MiningBase(BaseLocation* newLocation)
{
	location = newLocation;
	refinery = NULL;
	minersPerPatch = 2.f;
	active = false;
}

MiningBase::~MiningBase(void)
{

}

void MiningBase::init(vector<Unit*>* works)
{
	workers = works;
}

void MiningBase::manageBase()
{
	bool hasCC = false;
	set<Unit*> units = Broodwar->getUnitsOnTile(location->getTilePosition().x(),location->getTilePosition().y());
	for(set<Unit*>::iterator m = units.begin(); m != units.end(); m++)
	{
		if((*m)->getType().isResourceDepot() && (*m)->getPlayer() == Broodwar->self())
		{
			hasCC = true;
		}
	}

	if(!hasCC)
	{
		if(active)
		{
			active = false;
		}
	}
	else
	{
		if(!active)
			active = true;
	}

	if(active)
	{
		//do any active tasks
		if(refinery == NULL)
		{
			if(!location->isMineralOnly())
			{
				if((*location->getGeysers().begin())->getPlayer() == Broodwar->self())
				{
					refinery = new Refinery(*location->getGeysers().begin());
				}
			}
		}
	}
	else
	{
		//set mining workers to idle if any;
	}

	if(refinery)
	{
		refinery->manageRefinery();
		if(!refinery->inExistence())
		{
			delete refinery;
			refinery = NULL;
		}
	}
}
void MiningBase::sendMiner(Unit* newMiner)
{
	Unit* mineral = *location->getMinerals().begin();
	if(mineral != NULL)
		newMiner->rightClick(mineral);
}
void MiningBase::setMPP(float newMPP)
{
	minersPerPatch = newMPP;
}
void MiningBase::setGas(int newGas)
{
	if(refinery != NULL)
		refinery->setMax(newGas);
}
void MiningBase::setSomeIdle()
{
	vector<Unit*> toBeIdled;
	int count = getApproxMiners() - getMaxMinerCount();
	set<Unit*>::iterator m = miners.begin();
	for(int c = 0; c < count; c++)
	{
		while(!((*m)->exists() && (*m)->isGatheringMinerals() && (*m)->getTargetPosition().getDistance(location->getPosition()) < 500))
		{
			m = miners.erase(m); // Erase and increment to the next element
			if(m == miners.end())
				break;
			//m++;
		}
		if(m == miners.end())
			break;

		if((*m)->isGatheringMinerals())
			(*m)->stop();
		
		m++;
	}
}
int MiningBase::getApproxMiners()
{
	int count = 0;
	for(unsigned int c = 0; c < workers->size(); c++)
	{
		if((*workers)[c]->exists())
		{
			if((*workers)[c]->isGatheringMinerals() && (*workers)[c]->getTargetPosition().getDistance(location->getPosition()) < 500)
			{
				count++;
				miners.insert((*workers)[c]);
			}
		}
	}
	return count;
}
int MiningBase::getMaxMinerCount()
{
	int count = 0;
	set<Unit*> mins = location->getMinerals();
	set<Unit*>::iterator c;
	for(c = mins.begin(); c != mins.end(); c++)
	{
		if((*c) != NULL)
			if((*c)->exists())
				count ++;
	}
	patchCount = count;
	count = (int)(count * minersPerPatch);
	if(refinery)
		count += 3;
	return count;
}
int MiningBase::getDistanceFrom(Position fromPosition)
{
	return (int)fromPosition.getDistance(location->getPosition());
}
int MiningBase::getNumPatches()
{
	return patchCount;
}
bool MiningBase::isEmpty()
{
	if(location->minerals() == 0)
		return true;
	return false;
}
bool MiningBase::isActive()
{
	manageBase();	// Updates active
	return active;
}
bool MiningBase::hasGasAvailable()
{
	manageBase();	// Updates refinery
	if(refinery == NULL && !location->isMineralOnly())
	{
		return true;
	}
	return false;
}
bool MiningBase::hasRefinery()
{
	if(refinery)
	{
		return true;
	}
	return false;
}
bool MiningBase::isGasSaturated()
{
	if(refinery)
		return refinery->isSat();
	return true;
}
TilePosition MiningBase::getGasPosition()
{	
	return (*location->getGeysers().begin())->getTilePosition();
}