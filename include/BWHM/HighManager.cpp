#include <BWHM\HighManager.h>

Unit* baseWall;

void HighManager::onStart() 
{

	ArmyModule::init();
	ProductionModule::init();
	//ScoutingModule::init();

	AM::AID idleSquad = ArmyModule::createSquad();
	AIDMap["IdleSquad"] = idleSquad;	

	/*quick implementation*/
	run = true;
	autoCamera = false;
	ProductionModule::addToOrder(UnitTypes::Terran_Marine, 4);

}

int framesSince = 0;
void HighManager::onFrame() 
{

	// Thoughts:
	//	It would be really nice if the following:
	//		isEnemyNear(pos)
	//		various things relative to our starting location (ie, perhaps let players write sripts assuming a given start location, and translate for them)
	//		? getAIDIterator()
	//		Done! Ish! -- expand tasks need to not execute every frame and thereby stop it from actually /landing/ the CC
	//		Defend our main, then our expansion, etc. Fall back to defend against attacks.
	//		The AIUtility functions are prone to index errors if we go past the max number of bases on the map
	//			Change them to functions rather than direct access and return the last base for all bases past the last one
	//		Need to sweep enemy bases when we tell a squadron to attack there. IE, we need "attack" armytask.
	//		Done! Ish! Oi! -- **Expansions should mine their gas if needed
	//		Done! Mostly! -- Null pointers >_<
	//		**Tanks stop all production unless there is a machine shop on every factory, and it doesn't automatically build those machine shops
	//		Done! Although tanks are still borked -- **Check production dependencies so all production doesn't halt when a building breaks
	//		**Army should stop to fight if it intercepts enemies.
	//		Done! Upgrades should work as well (weapons / armor)
	//		****Medics keep getting in the way..
	//		Done! I think! -- The game shouldn't crash when we've mined out the map...

	//		Done! Ish! We could still really use a simple comsat interface. -- Detection!!!!
	//		Units to get out of each other's way
	//		Better way to find buildings / determine successful attack
	//		Done! Game is now slowing down noticably as the game progresses
	//		We're now over-saturating refineries
	//		Done! Add-ons are not being rebuilt after being destroyed (I think), are not being built on all needed buildings, and etc etc
	//		I think factories and such are not being double used as a dependency and a production facility?
	//		If production is held up on any unit (ie, starport is destroyed), all production stops and waits for that
	
	//		Use tech
	//		Load/unload dropships

	//ScoutingModule::onFrame();

	ProductionModule::onFrame();

	std::stringstream frameCount;
	frameCount << "Frame: " << BWAPI::Broodwar->getFrameCount();
	BWAPI::Broodwar->drawTextScreen(0,10,frameCount.str().c_str());
	
	static BWTA::BaseLocation *startingBase = BWTA::getStartLocation(BWAPI::Broodwar->self());
		
	static int numberOfBases = 1;
	
	static std::set<AM::AID> dynamicSquadSet;
	static bool startedBottom = (BWTA::getStartLocation(BWAPI::Broodwar->self())->getPosition().y() > 400);
	static BWAPI::Position wayPoint = (startedBottom ? BWAPI::Position(42*32,38*32) : BWAPI::Position(54*32,90*32));
	static BWAPI::Position enemyBase = (startedBottom ? BWAPI::Position(32*32,8*32) : BWAPI::Position(64*32,120*32));
	wayPoint = AIUtilities::expansionVector[0]->getPosition();
	
	if(framesSince < 12)
	{
		framesSince++;
	}
	else
	{
		framesSince = 0;

		ArmyModule::clearNonexistentUnits();

		if(BWAPI::Broodwar->getFrameCount() > (unsigned int) 100000)
		{
			BWAPI::Broodwar->restartGame();
		}

		// Automated camera movement
		static int cameraTimer = 60;
		static int cameraCurrent = 0;
		if (autoCamera == true && BWAPI::Broodwar->elapsedTime() >= cameraTimer)
		{
			cameraTimer += 60;
			bool moved = false;
			for (unsigned int i = 0; i < AIUtilities::expansionVector.size() && !moved; i++)
			{
				cameraCurrent = (cameraCurrent + 1) % (AIUtilities::expansionVector.size());
				if (BWAPI::Broodwar->isVisible(AIUtilities::expansionVector[cameraCurrent]->getTilePosition()))
				{
					// Camera offset for bases determined empirically as the following
					BWAPI::Broodwar->setScreenPosition(AIUtilities::expansionVector[cameraCurrent]->getPosition().x() - 320,
													   AIUtilities::expansionVector[cameraCurrent]->getPosition().y() - 240);
					moved = true;
					//BWAPI::Broodwar->printf("Screen: (%d,%d) Base: (%d,%d)",BWAPI::Broodwar->getScreenPosition().x(),BWAPI::Broodwar->getScreenPosition().y(),AIUtilities::expansionVector[0]->getPosition().x(),AIUtilities::expansionVector[0]->getPosition().y());
				}
			}
		}
	
		ArmyModule::task(AIDMap["IdleSquad"], "Attack");
		ArmyModule::setObjectivePosition(AIDMap["IdleSquad"], AIUtilities::expansionVector[0]->getPosition());
		ArmyModule::doTask(AIDMap["IdleSquad"]);
		ArmyModule::updateTeamMicro(AIDMap["IdleSquad"]);
			
		static int expansionTimer = 60*6; //First expansion at the 6 minute mark
		if(BWAPI::Broodwar->elapsedTime() >= expansionTimer && (unsigned int)numberOfBases < AIUtilities::expansionVector.size()-1)
		{
			bool rebuiltOld = false;
			for (int i = 0; i < numberOfBases && !rebuiltOld; i++) 
			{
				bool emptyBaseFound = true;
				std::set<BWAPI::Unit*> unitsAtPosition = BWAPI::Broodwar->getUnitsInRadius(AIUtilities::expansionVector[i]->getPosition(),1);
				for(std::set<BWAPI::Unit*>::const_iterator j=unitsAtPosition.begin(); 
					j != unitsAtPosition.end() && emptyBaseFound; 
					j++)
				{
					if((*j) && (*j)->exists() && (*j)->getType().isResourceDepot()) 
					{ 
						
						emptyBaseFound = false;
					}
				}
				if (emptyBaseFound)
				{
					//BWAPI::Broodwar->printf("Rebuilding base number %d of %d", i, AIUtilities::expansionVector.size()-1);
					ProductionModule::expandHere(AIUtilities::expansionVector[i]->getTilePosition(), true);
					rebuiltOld = true;
					expansionTimer += 60*6; // Take the next expansion 6 minute from now
				}
			}
			if (!rebuiltOld)
			{
				//BWAPI::Broodwar->printf("Taking base number %d of %d", numberOfBases, AIUtilities::expansionVector.size()-1);
				ProductionModule::expandHere(AIUtilities::expansionVector[numberOfBases]->getTilePosition(), true);
				numberOfBases++;
				expansionTimer += 60*6; // Take next expansion 6 minutes from now
			}
		}
	
		static int waveSize = 3;
		static int phase = 1;
		static int targetBase = 0;
		if(numberOfBases == 2 && phase == 1)
		{
			ProductionModule::addToOrder(UnitTypes::Terran_Firebat, 1);
			waveSize = 4;
			phase++;
		}

		if(numberOfBases == 3 && phase == 2) 
		{
			ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Armor);
			ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Weapons); 
			ProductionModule::maintainThisUnit(BWAPI::UnitTypes::Terran_Medic, 10);
			ProductionModule::maintainThisUnit(BWAPI::UnitTypes::Terran_Science_Vessel, 4);
			
			//ProductionModule::addToOrder(UnitTypes::Terran_Siege_Tank_Tank_Mode, 1); // Works! Might be floating resources a bit
			//ProductionModule::addToOrder(UnitTypes::Terran_Ghost, 1); // Works!
			//ProductionModule::addToOrder(UnitTypes::Terran_Vulture, 1); // Wokrs! Might be floating resources a bit?
			//ProductionModule::addToOrder(UnitTypes::Terran_Wraith, 1); // Works!
			//ProductionModule::addToOrder(UnitTypes::Terran_Battlecruiser, 1); // Works!
			//ProductionModule::addToOrder(UnitTypes::Terran_Valkyrie, 1); // Works!
			//ProductionModule::addToOrder(UnitTypes::Terran_Dropship, 1); // Works!
			//ProductionModule::addToOrder(UnitTypes::Terran_Goliath, 1); // Works!
			waveSize = 6;
			phase++;
		}
		if(numberOfBases == 4 && phase == 3)
		{
			ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Armor);
			ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Weapons); 
			ProductionModule::maintainThisUnit(BWAPI::UnitTypes::Terran_Medic, 20);
			phase++;
		}
		if(numberOfBases == 5 && phase == 4)
		{
			ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Armor);
			ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Weapons); 
			
			phase++;
		}
		if(phase == 5)
		{
			if(BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Infantry_Armor) < 3)
			{
				//ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Armor);
			}
			if(BWAPI::Broodwar->self()->getUpgradeLevel(BWAPI::UpgradeTypes::Terran_Infantry_Weapons) < 3)
			{
				//ProductionModule::upgrade(BWAPI::UpgradeTypes::Terran_Infantry_Weapons);
			}
		}

		for(std::set<AM::AID>::const_iterator i = dynamicSquadSet.begin(); i != dynamicSquadSet.end(); i++)
		{
			if((ArmyModule::taskFailed((*i)) || ArmyModule::getTaskName((*i)) == "Idle") 
				&& ArmyModule::getTotalOfType(AIDMap["IdleSquad"], BWAPI::UnitTypes::Terran_Marine) >= 4*waveSize)
			{
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Marine, waveSize*4);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Medic, waveSize*4);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, waveSize*4);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Firebat, waveSize*4);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Ghost, waveSize*4);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Vulture, waveSize*4);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Wraith, waveSize*4);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Science_Vessel, 1);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Battlecruiser, 1);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Valkyrie, 1);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Dropship, 1);
				ArmyModule::transferTo(AIDMap["IdleSquad"],(*i), BWAPI::UnitTypes::Terran_Goliath, waveSize*4);

				ArmyModule::task((*i), "MoveOut");
				ArmyModule::setObjectivePosition((*i), wayPoint); //10
				ArmyModule::doTask((*i));
			}

			if(ArmyModule::getTaskName((*i)) == "MoveOut" && ArmyModule::taskComplete((*i)))
			{
				ArmyModule::task((*i), "Attack");
				ArmyModule::setObjectivePosition((*i), AIUtilities::expansionVector[targetBase % AIUtilities::expansionVector.size()]->getPosition()); //11
				ArmyModule::doTask((*i));
			}

			if(ArmyModule::getTaskName((*i)) == "Attack" && ArmyModule::taskComplete((*i)))
			{
				targetBase++;
				//BWAPI::Broodwar->printf("Attack complete! Now targeting base: %d of %d (%d)", (targetBase % AIUtilities::expansionVector.size()), AIUtilities::expansionVector.size()-1, targetBase);
				ArmyModule::task((*i), "Attack");
				ArmyModule::setObjectivePosition((*i), AIUtilities::expansionVector[targetBase % AIUtilities::expansionVector.size()]->getPosition()); //11
				ArmyModule::doTask((*i));
			}

			if(ArmyModule::getTaskName((*i)) == "Attack" && ArmyModule::taskFailed((*i)))
			{
				targetBase--;
				//BWAPI::Broodwar->printf("Attack failed! Falling back to base: %d of %d", (targetBase % AIUtilities::expansionVector.size()), AIUtilities::expansionVector.size()-1);
				ArmyModule::task((*i), "Idle");
			}

			if(ArmyModule::taskInProgress((*i)))
			{
				ArmyModule::updateTeamMicro((*i));
			}
		}
	
		while(ArmyModule::getTotalOfType(AIDMap["IdleSquad"], BWAPI::UnitTypes::Terran_Marine) >= 4*waveSize)
		{
			AM::AID newSquad = ArmyModule::createSquad();
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Marine, waveSize*4);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Medic, waveSize);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode, waveSize);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Firebat, waveSize);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Ghost, waveSize);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Vulture, waveSize);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Wraith, waveSize);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Science_Vessel, 1);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Battlecruiser, 1);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Valkyrie, 1);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Dropship, 1);
			ArmyModule::transferTo(AIDMap["IdleSquad"], newSquad, BWAPI::UnitTypes::Terran_Goliath, waveSize);
			ArmyModule::task(newSquad, "MoveOut");
			ArmyModule::setObjectivePosition(newSquad, wayPoint); //10
			ArmyModule::doTask(newSquad);
			dynamicSquadSet.insert(newSquad);
		}
	} 
}

void HighManager::onSendText(std::string text)
{
	if (text == "/camera" || text == "/c")
	{
		BWAPI::Broodwar->printf("Autocamera has been enabled.");
		autoCamera = true;
	}
	if (text == "/stopcamera" || text == "/sc")
	{
		BWAPI::Broodwar->printf("Autocamera has been disabled.");
		autoCamera = false;
	}
	

}

void HighManager::onUnitCreate(BWAPI::Unit* unit)
{
	ProductionModule::onUnitCreate(unit);

}

void HighManager::onUnitComplete(BWAPI::Unit* unit) 
{
	ProductionModule::onUnitComplete(unit);
	ArmyModule::addUnit(AIDMap["IdleSquad"], unit);
}

void HighManager::onUnitDestroy(BWAPI::Unit* unit) 
{
	ArmyModule::decommissionUnit(unit);
	ProductionModule::onUnitDestroy(unit);
	//ScoutingModule::onUnitDestroy(unit);
}

void HighManager::onUnitDiscover(BWAPI::Unit* unit)
{
	//ScoutingModule::onUnitDiscover(unit);
}

void HighManager::onUnitHide(BWAPI::Unit* unit) {}

void HighManager::onUnitEvade(BWAPI::Unit* unit)
{
	//ScoutingModule::onUnitEvade(unit);
}


