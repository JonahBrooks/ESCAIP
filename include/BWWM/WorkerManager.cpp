#include "WorkerManager.h"


WorkerManager::WorkerManager(void)
{
	MiningBase::init(&workers);
	frameCount = 0;
	saturated = false;
	defaultMPP = 2;
	set<BaseLocation*> allBases = BWTA::getBaseLocations();
	set<BaseLocation*>::iterator c;
	MiningBase* temp;
	for(c = allBases.begin(); c != allBases.end(); c++)
	{
		if((*c) != NULL)
		{
			temp = new MiningBase(*c);
			temp->setMPP(defaultMPP);
			bases.push_back(temp);
		}
	}
}

WorkerManager::~WorkerManager(void)
{
}

void WorkerManager::setDefaultMPP(float newMPP)
{
	defaultMPP = newMPP;
	int size = bases.size();
	for(int c = 0; c < size; c++)
	{
		bases[c]->setMPP(defaultMPP);
	}
}
void WorkerManager::addWorker(Unit* newWorker)
{
	if(newWorker->getType().isWorker() && newWorker->getPlayer() == Broodwar->self())
		workers.push_back(newWorker);
}
void WorkerManager::removeWorker(Unit* deadWorker)
{
	//if(deadWorker->getPlayer() == Broodwar->self() && deadWorker->getType().isWorker())
		
	for(unsigned int c = 0; c < workers.size(); c++)
		if(workers[c] == deadWorker)
		{
			workers.erase(workers.begin() + c);
		}
}
void WorkerManager::manage()
{
	frameCount++;
	if(frameCount > 30)
	{
		frameCount = 0;
		bool tempSat = true;
		for(unsigned int c = 0; c < bases.size(); c++)
		{
			bases[c]->manageBase();
			if(bases[c]->getApproxMiners() < bases[c]->getMaxMinerCount())
				tempSat = false;
			if(bases[c]->getApproxMiners() > bases[c]->getMaxMinerCount() && saturated == false)
				bases[c]->setSomeIdle();
		}
		saturated = tempSat;

		int size = workers.size();
		for(int c = 0; c < size; c++)
		{
			if(workers[c]->isIdle())
			{
				MiningBase* closest = NULL;
				int distance = 99999999;
				for(unsigned int i = 0; i < bases.size(); i++)
				{
					if(bases[i]->isActive())
						if(bases[i]->getApproxMiners() < bases[i]->getMaxMinerCount())
						{
							if(bases[i]->getDistanceFrom(workers[c]->getPosition()) < distance)
							{
								if(!bases[i]->isEmpty())
								{
									distance = bases[i]->getDistanceFrom(workers[c]->getPosition());
									closest = bases[i];
								}
							}
							//Broodwar->printf("GG");
							//bases[i]->sendMiner(workers[c]);
						}
				}
				if(closest != NULL)
					closest->sendMiner(workers[c]);
				else
				{
					//Broodwar->printf("ERROR NO BASE FOUND GG GET DUNKED");
					Unit* closestMineral=NULL;
					for(std::set<Unit*>::iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
					{
						if (closestMineral == NULL || workers[c]->getDistance(*m) < workers[c]->getDistance(closestMineral))
							closestMineral = *m;
					}
					if (closestMineral != NULL)
						workers[c]->rightClick(closestMineral);
				}
			}
		}
	}
}
int WorkerManager::getWorkerMax()
{
	int count = 0;
	for(unsigned int c = 0; c < bases.size(); c++)
	{
		if(bases[c]->isActive())
			count += bases[c]->getMaxMinerCount();
	}
	return count;
}
int WorkerManager::getAdjustedMinerCount()
{
	int totalPatches = 0;
	int totalMiners = 0;
	for(unsigned int c = 0; c < bases.size(); c++)
	{
		totalPatches += bases[c]->getNumPatches();
	}
	for(unsigned int c = 0; c < workers.size(); c++)
	{
		if(workers[c]->isGatheringMinerals())
			totalMiners ++;
	}
	if(totalMiners <= 2 * totalPatches)
		return totalMiners;
	else
		return 2 * totalPatches;
}
int WorkerManager::getAdjustedRefinerCount()
{
	int totalGeysers = 0;
	int totalRefiners = 0;
	for(unsigned int c = 0; c < bases.size(); c++)
	{
		if(bases[c]->hasRefinery())
			totalGeysers ++;
	}
	for(unsigned int c = 0; c < workers.size(); c++)
	{
		if(workers[c]->isGatheringGas())
			totalRefiners ++;
	}
	if(totalRefiners <= 3 * totalGeysers)
		return totalRefiners;
	else
		return totalGeysers * 3;
}
TilePosition WorkerManager::getNewGasPosition()
{
	for(unsigned int c = 0; c < bases.size(); c++)
	{
		if(bases[c]->isActive() && bases[c]->hasGasAvailable() )
		//if(bases[c]->getGasPosition())
		{
			if( !(bases[c]->getGasPosition().x() == 1000 && bases[c]->getGasPosition().y() == 1002))
			{	// (1000,1002) seems to be the tile position of invalid geysers
				return bases[c]->getGasPosition();
			}
		}
	}
	return TilePosition(0,0);
}

bool WorkerManager::canShiftToMins()
{
	return !saturated;
}

bool WorkerManager::canShiftToGas()
{
	for(unsigned int c = 0; c < bases.size(); c++)
	{
		if(!bases[c]->isGasSaturated())
			return true;
	}
	return false;
}