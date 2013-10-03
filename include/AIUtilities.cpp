#include <AIUtilities.h>

BWAPI::AIModule *AIUtilities::module;
std::set<BWAPI::Unit*> AIUtilities::geysers;

std::vector<BWTA::BaseLocation*> AIUtilities::expansionVector;
std::vector<BWTA::BaseLocation*> AIUtilities::enemyExpansionVector;

std::map<std::string, BWTA::BaseLocation*> AIUtilities::expansionMap;
std::map<std::string, BWTA::BaseLocation*> AIUtilities::enemyExpansionMap;

void AIUtilities::init(BWAPI::AIModule *moduleArg)
{  
	module = moduleArg;
    std::priority_queue<BWTA::BaseLocation*, std::vector<BWTA::BaseLocation*>, BaseLocationCompare> expansionQueue (BWTA::getStartLocation(BWAPI::Broodwar->self()));
    std::priority_queue<BWTA::BaseLocation*, std::vector<BWTA::BaseLocation*>, BaseLocationCompare> enemyExpansionQueue (BWTA::getStartLocation(BWAPI::Broodwar->enemy()));


    for(std::set<BWTA::BaseLocation*>::const_iterator base = BWTA::getBaseLocations().begin(); base != BWTA::getBaseLocations().end(); base++) 
    {
        expansionQueue.push((*base));
        enemyExpansionQueue.push((*base));
    }

    expansionVector.resize(expansionQueue.size());
    enemyExpansionVector.resize(enemyExpansionQueue.size());

    int i = 0;
    while(expansionQueue.empty() == false)
    {
        expansionVector[i] = expansionQueue.top();
        expansionQueue.pop();
		enemyExpansionVector[i] = enemyExpansionQueue.top();
        enemyExpansionQueue.pop();
        i++;
    }

}

void AIUtilities::trackGeyser(BWAPI::Unit *geyser)
{
	geysers.insert(geyser);
}

void AIUtilities::onFrame()
{
	// Calculate onUnitComplete for refineries, since BWAPI/Broodwar does not do this
	// No need to check this every frame, or when there are no geysers to check
	if(BWAPI::Broodwar->getFrameCount() % 32 == 0 && geysers.size() != 0)
	{
		std::set<BWAPI::Unit*>::iterator i = geysers.begin();
		while(i != geysers.end())
		{
			if(!(*i))
			{
				i = geysers.erase(i);
			}
			else if (!(*i)->exists())
			{
				i = geysers.erase(i);
			}
			else if((*i)->getRemainingBuildTime() == 0)
			{
				module->onUnitComplete(*i);
				i = geysers.erase(i);
			}
			else
			{
				i++;
			}
		}
	}

	// Old method. Thorough, but excessive and buggy.
	// // Calculate onUnitComplete for refineries, as BWAPI doesn't seem to do this.
	//static std::map<BWAPI::Unit*,bool> isComplete;
	//for(std::set<BWTA::BaseLocation*>::const_iterator base = BWTA::getBaseLocations().begin(); base != BWTA::getBaseLocations().end(); base++) 
    //{
	//	if((*base) && BWAPI::Broodwar->isVisible((*base)->getTilePosition()) && !(*base)->isMineralOnly())
	//	//if((*base) && *(*base)->getGeysers().begin())
	//	{
	//		int x = (*(*base)->getGeysers().begin())->getTilePosition().x();
	//		int y = (*(*base)->getGeysers().begin())->getTilePosition().y();
	//		if (x < 1000 && y < 1000)
	//		{
	//			std::set<BWAPI::Unit*> onTile = BWAPI::Broodwar->getUnitsOnTile(x,y);
	//			for(std::set<BWAPI::Unit*>::const_iterator unitIt = onTile.begin(); unitIt != onTile.end(); unitIt++)
	//			{
	//				if((*unitIt) && (*unitIt)->exists() && (*unitIt)->getPlayer() == BWAPI::Broodwar->self())
	//				{
	//					if ((*unitIt)->getRemainingBuildTime() == 0 && !isComplete[(*unitIt)])
	//					{
	//						module->onUnitComplete(*unitIt);
	//					}
	//					isComplete[(*unitIt)] = (*unitIt)->getRemainingBuildTime() == 0;
	//				}
	//			}
	//		}
	//		else
	//		{
	//			//BWAPI::Broodwar->printf("Ignoring invalid geyser location. Total valid Geysers tracked: %d", isComplete.size());
	//		}
	//	}
    //}



}
