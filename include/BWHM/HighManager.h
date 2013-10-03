#pragma once
#include <stdlib.h>
#include <string>
#include <sstream>

#include <BWAPI.h>
#include <BWTA.h>
#include <BWAM\ArmyModule.h>
#include <BWPM\ProductionModule.h>
#include <BWSM\ScoutingModule.h>

#include <AIUtilities.h>

 /** @file HighManager.h
  *
  *	 @author Programming of this file and HighManager.cpp: Jonah Brooks
  *	 @author Doxygen documentation of this file and HighManager.cpp: Savannah Van Beek and Jonah Brooks
  */

class HighManager {
private:
	std::map<std::string, AM::AID> AIDMap;
	bool run;
	bool autoCamera;

public:
   /**   
	*	Called by the BWAPI at the start of the match
	*	
	*	@see BWAPI::AIModule::onStart()
	*
	*	@param none
	*	@return none
	*/
	void onStart();

   /**   
	*	Called by the BWAPI on every game frame
	*	
	*	@see BWAPI::AIModule::onFrame()
	*
	*	@param none
	*	@return none
	*/
	void onFrame();

   /**
    *	Called by the BWAPI whenever text is sent via the in-game chat
	*
	*	@see BWAPI::AIModule::onSendText()
	*
	*	@param text sent via the in-game chat
	*	@return none
	*/
	void onSendText(std::string text);

   /**   
	*	Called by the BWAPI whenever a new unit is created
	*	
	*	@see BWAPI::AIModule::onUnitCreate()
	*	
	*	@param unit that triggered this event
	*	@return none
	*/
	void onUnitCreate(BWAPI::Unit *unit);

   /**   
	*	Called by the BWAPI whenever the production of a unit is complete
	*	
	*	@see BWAPI::AIModule::onUnitComplete()
	*
	*	@param unit that triggered this event
	*	@return none
	*/
	void onUnitComplete(BWAPI::Unit *unit);

   /**   
	*	Called by the BWAPI whenever a unit is destroyed
	*	
	*	@see BWAPI::AIModule::onUnitDestroy()
	*
	*	@param unit that triggered this event
	*	@return none
	*/
	void onUnitDestroy(BWAPI::Unit *unit);

   /**   
	*	Called by the BWAPI whenever a unit enters a visible area
	*	
	*	@see BWAPI::AIModule::onUnitDiscover()
	*
	*	@param unit that triggered this event
	*	@return none
	*/
	void onUnitDiscover(BWAPI::Unit *unit);

   /**   
	*	Called by the BWAPI whenever a unit exits a visible area
	*	
	*	@see BWAPI::AIModule::onUnitHide()
	*
	*	@param unit that triggered this event
	*	@return none
	*/
	void onUnitHide(BWAPI::Unit *unit);

   /**   
	*	Called by the BWAPI whenever a unit becomes inaccessible
	*	
	*	@see BWAPI::AIModule::onUnitEvade()
	*	
	*	@param unit that triggered this event
	*	@return none
	*/
	void onUnitEvade(BWAPI::Unit *unit);

};