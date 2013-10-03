#pragma once
#include <map>
#include <string>
#include <BWAPI.h>
#include <BWTA.h>

#include <BWAM\Fireteams\Fireteam.h>
#include <BWAM\ArmyTasks\ArmyTask.h>
#include <BWAM\ArmyTasks\Specific\SpecificArmyTasks.h>
#include <BWAM\Squadron.h>

 /** @file ArmyModule.h
  *  @namespace AM 
  *
  *	 @author Programming of this and all other BWAM files: Jonah Brooks
  *	 @author Documentation of this and all other BWAM files: Savannah Van Beek and Jonah Brooks
  */

namespace AM {
	/** 
	 *	@typedef AID
	 *	
	 *	An identifier used to identify a Squadron.
	 *	Allows future users to change how armies are managed without breaking compatability with existing scripts.
	 */
	typedef Squadron* AID;
}

/** 
 *	@class ArmyModule
 *
 *	A singleton class used as an adapter to the military management files.
 * 
 */
class ArmyModule {

private:
	
	ArmyModule() {};
	static std::map<AM::AID, Squadron*> armyMap;	/**< Maps each army ID (AID) to its corresponding Squadron. */

public:
	
	/** 
	 *	Initializes module prerequisites, such as each Specific Army Task.
	 *	
	 *	@param none
	 *	@return none
	 */

	static void init();

	static void clearNonexistentUnits() { Squadron::clearNonexistentUnits(); }

	/** 
	 *	Creates a new Squadron, returning the new AID as well as storing it in armyMap.
	 *	
	 *	@see AM::AID
	 *
	 *	@param none
	 *	@return an AM::AID identifying the newly created Squadron
	 *  @todo	possibly add a deleteSquad function?
	 */
	static AM::AID createSquad();
	
	/** 
	 *	Notifies the module that a unit needs to be removed.
	 *	
	 *	@param unit The unit that is to be removed.
	 *	@return none
	 */	
	static void decommissionUnit(BWAPI::Unit*);

	/** 
	 *	Transfers up to \a num units of type \a unitType from \a idFrom to \a idTo.
	 *	
	 *	@param idFrom the AID of the Squadron from which units should be transfered.
	 *	@param idTo the AID of the Squadron to which units should be transfered.
	 *	@param unitType the type of unit that should be transfered between \a idFrom and \a idTo.
	 *	@param num the number of units that should be transfered between \a idFrom and \a idTo.
	 *	@return none
	 */
	static void transferTo(AM::AID idFrom, AM::AID idTo, BWAPI::UnitType unitType, int num); 
	
	/** 
	 *	Transfers all units from \a idFrom to \a idTo, leaving \a idFrom's Squadron empty.
	 *	
	 *	@param idFrom the AID of the Squadron from which all units should be transfered.
	 *	@param idTo the AID of the Squadron to which these units should be transfered.
	 *	@return none
	 *  @todo What do we want to do with the empty Squadron?
	 */
	static void mergeWith(AM::AID idFrom, AM::AID idTo); 

	/** 
	 *	Adds \a unit to \a id's Squadron for automatic allocation.
	 *	
	 *	@param id the AID of the Squadron to which \a unit should be added. 
	 *	@param unit the unit that is to be added to \a id's Squadron.
	 *	@return none
	 */
	static void addUnit(AM::AID id, BWAPI::Unit* unit); 

	/** 
	 *	Returns the total number of units of type \a unitType accross all Fireteams contained in \a id.
	 *	
	 *	@param id the AID of the Squadron from which units of type \a unitType should be counted. 
	 *	@param unitType the type of unit which should be counted.
	 *	@return The number of units of type \a unitType accross all Fireteams contained in \a id.
	 */
	static int getTotalOfType(AM::AID id, BWAPI::UnitType unitType); 
	
	/** 
	 *	Checks to see if \a id is currently in the process of completing its task.
	 *	
	 *	@param id the AID of the Squadron whose task progress should be checked.
	 *	@return True if \a id's current task is in progress, False if not.
	 */
	static bool taskInProgress(AM::AID id);

	/** 
	 *	Checks to see if \a id has completed its current task.
	 *	
	 *	@param id the AID of the Squadron whose task progress should be checked.
	 *	@return True if \a id's current task is completed, False if not.
	 */
	static bool taskComplete(AM::AID id);
	
	/** 
	 *	Checks to see if \a id has failed its current task.
	 *	
	 *	@param id the AID of the Squadron whose task progress should be checked.
	 *	@return True if \a id's current task is in a fail state, False if not.
	 */
	static bool taskFailed(AM::AID id);
	
	/** 
	 *	Returns the name of \a id's current task.
	 *	
	 *	@param id the AID of the Squadron whose task name is to be returned.
	 *	@return The name of the task \a id is currently assigned.
	 */
	static std::string getTaskName(AM::AID); 

	/** 
	 *	Issues a new \a taskName task to \a id. Sets taskInProgress, taskComplete, and taskFailed to false.
	 *	
	 *	@param id the AID of the Squadron which should be assigned a new task.
	 *	@param taskName the name of the task which should be assigned to \a id.
	 *	@return none
	 */
	static void task(AM::AID id, std::string taskName);	
	
	/** 
	 *	Orders \a id to perform its previously assigned task, unless the task is already in progress.
	 *	
	 *	@param id the AID of the Squadron whose task should now be performed.
	 *	@return none
	 *	@todo Return a bool indicating success (ie, false if current task is already completed/failed/inProgress)
	 */
	static void doTask(AM::AID);
	
	/** 
	 *	Orders \a id to update the position of each of its units.
	 *	
	 *	@param id the AID of the Squadron containing the units that need to be moved.
	 *	@return none
	 */
	static void updateTeamMicro(AM::AID id);
	
	/** 
	 *	Instructs \a id to perform its future task(s) on or at position \a pos.
	 *	
	 *	@param id the AID of the Squadron whose task should be performed at position \a pos.
	 *	@param pos the position at which \a id sould eventually perform its task.
	 *	@return none
	 *	@todo Return a bool indicating success (ie, false if \a pos is invalid)
	 */
	static void setObjectivePosition(AM::AID, BWAPI::Position);

	/** 
	 *	Returns the possition at which \a id has been ordered to complete its task.
	 *	
	 *	@param id the AID of the Squadron whose task position should be returned.
	 *	@return The position at which \a id has been ordered to complete its task.
	 */
	static BWAPI::Position getObjectivePosition(AM::AID id) { return armyMap[id]->getObjectivePosition(); }
};