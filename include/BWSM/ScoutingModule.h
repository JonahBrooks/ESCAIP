#pragma once

#include <BWSM\ScoutTaskManager.h>

 /** @file ScoutingModule.h 
  *
  *  @note Currently unstable and mostly unimplemented
  *
  *	 @author Programming of this file: Jonah Brooks
  *	 @author Doxygen documentation of this and all other BWSM files: Savannah Van Beek and Jonah Brooks
  *  @author Non-doxygen documentation and programming of all BWSM files: Joe Runde, Jesse Hostetler
  */

class ScoutingModule
{
private:
	static ScoutTaskManager *manager;
	static EnemyInfo *info;

	ScoutingModule() {};
public:

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static void init();

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static void addTask(BWTA::Region* place);

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static void onFrame();

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static void onUnitDiscover(Unit* unit);

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static void onUnitDestroy(Unit* unit);

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static void onUnitEvade(Unit* unit);

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static void resetScouting();

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static float getMins();

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static float getGas();

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static int getFirstDiscoveryTime(BWAPI::UnitType);

	/** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*/
	static int getBuildingAmount(BWAPI::UnitType);


	//TODO: ... maybe?

	//  int getRegionalBuildingAmount(BWAPI::UnitType, BWTA::Region); ?

	// startScouting() ?
	// stopScouting() ?

	// adjustInterval() ?

	// getUnitCount() ?
	// getArmyPosition() ?
	// getWeakPoints() ?

	// map<UnitType,int> getUnitEstimates(); ?

	// int getProductionTime(UnitType unit) ?

	// int getPercentageScouted() ?

	// Some way to iterate through regions ?

};

