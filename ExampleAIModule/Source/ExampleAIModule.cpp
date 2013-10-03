#include "ExampleAIModule.h"

#include <map>
#include <set>
#include <BWHM/HighManager.h>
#include <fstream>

using namespace BWAPI;

bool analyzed;
bool analysis_just_finished;
BWTA::Region* home;
BWTA::Region* enemy_base;
HighManager* manager;
//Squadron demoSquad = Squadron();

static int wins;
static int losses;

void ExampleAIModule::onStart()
{	
	BWTA::readMap();
	BWTA::analyze();
	Broodwar->enableFlag(Flag::UserInput);
	Broodwar->setLocalSpeed(0);

	AIUtilities::init(this);

	manager = new HighManager();
	manager->onStart();
	std::ifstream winlossIn;
	winlossIn.open("winlossrecord");
	if(winlossIn)
	{
		winlossIn >> wins;
		winlossIn >> losses;
		winlossIn.close();
	}
	else
	{
		wins = 0;
		losses = 0;
	}


}

void ExampleAIModule::onEnd(bool isWinner)
{
	if(isWinner)
	{
		wins++;
	}
	else
	{
		losses++;
	}

	std::ofstream winlossOut;
	winlossOut.open("winlossrecord");
	winlossOut << wins << " " << losses;
	winlossOut.close();
}

void ExampleAIModule::onFrame()
{
		std::stringstream gameResults(std::stringstream::in | std::stringstream::out);
		gameResults << "Wins: " << wins << " Losses: " << losses << " FPS: " << BWAPI::Broodwar->getFPS();
		BWAPI::Broodwar->drawTextScreen(0,0, gameResults.str().c_str());

		//std::stringstream fpsInfo;
		//fpsInfo << "FPS: " << BWAPI::Broodwar->getFPS();
		//BWAPI::Broodwar->drawTextScreen(100,0,fpsInfo.str().c_str());

		//drawTerrainData();

		AIUtilities::onFrame();

	
	manager->onFrame();
}

void ExampleAIModule::onSendText(std::string text)
{
	if(text == "/fast")
		Broodwar->setLocalSpeed(0);
	if(text == "/slow")
		Broodwar->setLocalSpeed(-1);
	if(text == "/frame")
		Broodwar->setLocalSpeed(200);
	if(text == "/med")
		Broodwar->setLocalSpeed(1);
	if(text == "/resetwinloss")
	{
		std::ofstream winlossOut;
		winlossOut.open("winlossrecord");
		winlossOut << "";
		wins = 0;
		losses = 0;
		winlossOut.close();
	}

	manager->onSendText(text);
}

void ExampleAIModule::onReceiveText(BWAPI::Player* player, std::string text)
{
  
}

void ExampleAIModule::onPlayerLeft(BWAPI::Player* player)
{
  
}

void ExampleAIModule::onNukeDetect(BWAPI::Position target)
{
  
}

void ExampleAIModule::onUnitDiscover(BWAPI::Unit* unit)
{
	manager->onUnitDiscover(unit);
}

void ExampleAIModule::onUnitEvade(BWAPI::Unit* unit)
{
	manager->onUnitEvade(unit);
}

void ExampleAIModule::onUnitShow(BWAPI::Unit* unit)
{
 
}

void ExampleAIModule::onUnitHide(BWAPI::Unit* unit)
{
 
}

void ExampleAIModule::onUnitCreate(BWAPI::Unit* unit)
{
	manager->onUnitCreate(unit);
}

void ExampleAIModule::onUnitDestroy(BWAPI::Unit* unit)
{
	manager->onUnitDestroy(unit);
}

void ExampleAIModule::onUnitMorph(BWAPI::Unit* unit)
{
	if(unit->getType() == BWAPI::UnitTypes::Terran_Refinery && unit->getPlayer() == BWAPI::Broodwar->self())
	{
		AIUtilities::trackGeyser(unit);
	}

}

void ExampleAIModule::onUnitRenegade(BWAPI::Unit* unit)
{
  
}

void ExampleAIModule::onSaveGame(std::string gameName)
{

}

DWORD WINAPI AnalyzeThread()
{
  BWTA::analyze();

  //self start location only available if the map has base locations
  if (BWTA::getStartLocation(BWAPI::Broodwar->self())!=NULL)
  {
    home       = BWTA::getStartLocation(BWAPI::Broodwar->self())->getRegion();
  }
  //enemy start location only available if Complete Map Information is enabled.
  if (BWTA::getStartLocation(BWAPI::Broodwar->enemy())!=NULL)
  {
    enemy_base = BWTA::getStartLocation(BWAPI::Broodwar->enemy())->getRegion();
  }
  analyzed   = true;
  analysis_just_finished = true;
  
  return 0;
}

void ExampleAIModule::drawStats()
{
  
}

void ExampleAIModule::drawBullets()
{
 
}

void ExampleAIModule::drawVisibilityData()
{
  
}

void ExampleAIModule::drawTerrainData()
{
  //we will iterate through all the base locations, and draw their outlines.
  for(std::set<BWTA::BaseLocation*>::const_iterator i=BWTA::getBaseLocations().begin();i!=BWTA::getBaseLocations().end();i++)
  {
    TilePosition p=(*i)->getTilePosition();
    Position c=(*i)->getPosition();

    //draw outline of center location
    Broodwar->drawBox(CoordinateType::Map,p.x()*32,p.y()*32,p.x()*32+4*32,p.y()*32+3*32,Colors::Blue,false);

    //draw a circle at each mineral patch
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getStaticMinerals().begin();j!=(*i)->getStaticMinerals().end();j++)
    {
      Position q=(*j)->getInitialPosition();
      Broodwar->drawCircle(CoordinateType::Map,q.x(),q.y(),30,Colors::Cyan,false);
    }

    //draw the outlines of vespene geysers
    for(std::set<BWAPI::Unit*>::const_iterator j=(*i)->getGeysers().begin();j!=(*i)->getGeysers().end();j++)
    {
      TilePosition q=(*j)->getInitialTilePosition();
      Broodwar->drawBox(CoordinateType::Map,q.x()*32,q.y()*32,q.x()*32+4*32,q.y()*32+2*32,Colors::Orange,false);
    }

    //if this is an island expansion, draw a yellow circle around the base location
    if ((*i)->isIsland())
      Broodwar->drawCircle(CoordinateType::Map,c.x(),c.y(),80,Colors::Yellow,false);
  }

  //we will iterate through all the regions and draw the polygon outline of it in green.
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    BWTA::Polygon p=(*r)->getPolygon();
    for(int j=0;j<(int)p.size();j++)
    {
      Position point1=p[j];
      Position point2=p[(j+1) % p.size()];
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Green);
    }
  }

  //we will visualize the chokepoints with red lines
  for(std::set<BWTA::Region*>::const_iterator r=BWTA::getRegions().begin();r!=BWTA::getRegions().end();r++)
  {
    for(std::set<BWTA::Chokepoint*>::const_iterator c=(*r)->getChokepoints().begin();c!=(*r)->getChokepoints().end();c++)
    {
      Position point1=(*c)->getSides().first;
      Position point2=(*c)->getSides().second;
      Broodwar->drawLine(CoordinateType::Map,point1.x(),point1.y(),point2.x(),point2.y(),Colors::Red);
    }
  }
}

void ExampleAIModule::showPlayers()
{
 
}

void ExampleAIModule::showForces()
{
  
}

void ExampleAIModule::onUnitComplete(BWAPI::Unit *unit)
{
	manager->onUnitComplete(unit);
}
