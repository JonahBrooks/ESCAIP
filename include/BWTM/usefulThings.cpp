#include "usefulThings.h"

void gatherMinerals(Unit* worker)
{
	Unit* closestMineral=NULL;
	for(std::set<Unit*>::iterator m=Broodwar->getMinerals().begin();m!=Broodwar->getMinerals().end();m++)
	{
		if (closestMineral == NULL || worker->getDistance(*m) < worker->getDistance(closestMineral))
			closestMineral = *m;
	}
	if (closestMineral != NULL)
		worker->rightClick(closestMineral);

}

void makeWorker(Unit* command)
{
	if(command->getType().getRace() != Races::Zerg)
		command->train(Broodwar->self()->getRace().getWorker());
	else
	{
		set<Unit*> allLarva = command->getLarva();
		if(allLarva.size() > 0)
		{
			Unit* larva = *allLarva.begin();
			larva->morph(UnitTypes::Zerg_Drone);
		}
	}
}

//void gatherGas
