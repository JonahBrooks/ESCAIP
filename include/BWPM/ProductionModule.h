#pragma once

//// TODO: Clean these up and standardize them. No quotes!
#include "ProductionOrder.h"
#include "ProductionDetails.h"
#include <BWPM\ProductionManager.h>
#include <BWTM\TaskManager.h>
#include <BWWM\WorkerManager.h>

 /** @file ProductionModule.h
  *
  *	 @author Programming of this file: Jonah Brooks
  *	 @author Doxygen documentation of this and all other BWPM, BWTM, and BWWM files: Savannah Van Beek and Jonah Brooks
  *  @author Non-doxygen documentation and programming of all BWPM, BWTM, and BWWM files: Joe Runde, modified and debuged by Jonah Brooks
  */

class ProductionModule
{
private:
	ProductionModule(void) { }
	~ProductionModule(void) { }

	static ProductionManager *singleton;
	static ProductionOrder *order;

public:

	/** 
	*	Initializes module's prerequisites.
	*	
	*	@param none
	*	@return none
	*/
	static void init();

	/** 
	*	Manages production tasks on each frame. Should be called from HighManager::onFrame().
	*	
	*	@param none
	*	@return none
	*/
	static void onFrame();

	/** 
	*	Handles various tasks upon the creation of a unit.
	*	
	*	@param unit that has just been created
	*	@return none
	*	@see	BWAPI::AIModule::onUnitCreate()
	*/
	static void onUnitCreate(Unit* unit);

	/** 
	*	Handles various tasks upon the destruction of a unit. 
	*	
	*	@param unit that has just been destroyed
	*	@return none
	*	@see	BWAPI::AIModule::onUnitDestroy()
	*/
	static void onUnitDestroy(Unit* unit);

	/** 
	*	Handles various tasks upon the completion of a unit.
	*	
	*	@param unit that has just been completed
	*	@return none
	*	@see	BWAPI::AIModule::onUnitComplete()
	*/
	static void onUnitComplete(Unit* unit);

	/** 
	*	Builds structures necessary for the construction of a specified \a unit (EX UnitTypes::Terran_Marine).
	*	
	*	@param unit whose dependencies should be resolved 
	*	@return none
	*/
	static void prepareTechFor(UnitType unit);

	/** 
	*	Builds structures necessary for the construction of a specified \a upgrade (EX: UpgradeTypes::Terran_Infantry_Weapons).
	*	
	*	@param upgrade whose dependencies should be resolved 
	*	@return none
	*/
	static void prepareTechFor(UpgradeType upgrade);

	/** 
	*	Builds structures necessary for the construction of a specified \a tech (EX: TechTypes::Stim_Packs).
	*	
	*	@param tech whose dependencies should be resolved 
	*	@return none
	*/
	static void prepareTechFor(TechType tech);

	/** 
	*	Produces all currently available upgrades for the given \a unit type.
	*	
	*	@param	unit type to be upgraded
	*	@return none
	*	@note	Does not construct buildings required for upgrades; only produces upgrades possible with current structures
	*/
	static void getAvailableUpgradesFor(UnitType unit);

	/** 
	*	Produces a constant \a amount of units of given \a unit type.
	*	If a unit of this type is destroyed then more are produce up to \a amount.
	*	
	*	@param unit type to be maintained
	*	@param amount of units to be maintained
	*	@return none
	*/
	static void maintainThisUnit(UnitType unit, int amount);

	/** 
	*	Stops production module from actively maintaining the amount of \a unit.
	*	
	*	@param unit type to stop maintaining
	*	@return none
	*/
	static void removeFromMaintenanceMap(UnitType unit);

	/** 
	*	Expands to the base location nearest to \a tPos.
	*	If ccNeeded is true this will also construct a new command center. 
	*	Otherwise it moves a non-active command center without building a new one.
	*	
	*	@param tPos Tile position near desired base location
	*	@param ccNeeded whether or not a new command center should be built
	*	@return none
	*/
	static void expandHere(TilePosition tPos, bool ccNeeded); 

	/** 
	*	Adds \a amount of \a unit type to the current production order.
	*	The production module will then attempt to use ratios to maintain unit composition.
	*	Example: addToOrder(UnitTypes::Terran_Marine, 3);
	*			 addToOrder(UnitTypes::Terran_Medic, 1);
	*	should produce 3 marines for every medic while continually producing units.
	*	
	*	@param unit type to be produced 
	*	@param amount to be added 
	*	@return none
	*/
	static void addToOrder(UnitType unit, int amount);

	/** 
	*	Unimplemented
	*	
	*	@param none
	*	@return none
	*/
	static void setAcceptableLosses(float minLosses, float gasLosses);

	/** 
	*	Unimplemented
	*	
	*	@param none
	*	@return none
	*/
	static void resetAcceptableLosses();

	/** 
	*	Produces \a upgrade if currently possible.
	*	If prepareTech is true this calls prepareTechFor before requested upgrade.
	*
	*	@param upgrade to be reserched
	*	@param prepareTech whether or not new buildings should be produced in order to research given upgrade
	*	@return true if upgrade can be produced immediately otherwise false
	*	@todo	If prepareTech is true this should research \a upgrade when dependencies are met
	*/
	static bool researchUpgrade(UpgradeType upgrade, bool prepareTech = true);

	/** 
	*	Produces \a tech if currently possible.
	*	If prepareTech is true this calls prepareTechFor before requested tech.
	*	
	*	@param tech to be researched
	*	@param prepareTech whether or not new buildings should be produced in order to research given tech
	*	@return true if tech can be produced immediately otherwise false
	*	@todo	If prepareTech is true this should research \a tech when dependencies are met
	*/
	static bool researchTech(TechType tech, bool prepareTech = true);

	/** 
	*	Convenience function for researchUpgrade 
	*	
	*	@param ups upgrade type
	*	@param prepareTech determines if tech should be built
	*	@return true if upgrade has been started
	*/
	static bool upgrade(UpgradeType ups, bool prepareTech = true);
	
	/** 
	*	Convenience function for researchTech 
	*	
	*	@param tech Tech type
	*	@param prepareTech determines if tech should be built
	*	@return true if tech has been started
	*/
	static bool research(TechType tech, bool prepareTech = true);


	/** 
	*	Produces one unit of \a unit type if currently possible. 
	*	
	*	@param unit type to be produced
	*	@return true if new unit can be produce immediately otherwise false
	*/
	static bool produceThis(UnitType unit);

	/** 
	*	Returns a map of all units currently being maintained.
	*	It stores everything that has been sent to maintainThisUnit().
	*	
	*	@param none
	*	@return current map mapping maintained units to maintained amount
	*/
	static map<UnitType,int> getMaintenanceMap();
};

