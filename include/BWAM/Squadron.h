#pragma once
#include <set>
#include <map>
#include <queue>
#include <string>
#include <BWAPI.h>
#include <BWTA.h>

#include <BWAM\Fireteams\Fireteam.h>
#include <BWAM\ArmyTasks\ArmyTask.h>

class Squadron {
private:
	
	static std::map<BWAPI::Unit*, Fireteam*> mapUnitToTeam; /** Maps each unit to the team to which they are assigned */
	static std::map<Fireteam*, Squadron*> mapTeamToSquad;   /** Maps each team to the squad to which they are assigned */
	std::set<Fireteam*> teams;	/** Set of teams within this squadron */

	ArmyTaskContext taskParameters;
	ArmyTask *currentTask;

	/**
	 *  @todo Is this needed
	 */
	void onUnitRemoved(BWAPI::Unit *unit, Fireteam *team);

public:
	/** 
	 *	Removes \a unit from mapUnitToTeam and from the team itself.
	 *  If the team is now destroyed, removes it from mapTeamToSquad and from the squad itself.
	 *
	 *  @warning This is the only function in which a unit should be removed from a team or squad. Doing otherwise may cause errors.
	 *	
	 *	@param unit that needs to be removed
	 *	@return none
	 */
	static void decommissionUnit(BWAPI::Unit *unit);

	static void clearNonexistentUnits();

	Squadron(); 

	/**
	 *	Orders the currentTask to distribute teams into different groups.
	 *
	 *	@see ArmyTask::adjustTeams()
	 *
	 *	@param none
	 *	@returns none
	 */
	void adjustTeams();

	/** 
	 *	Adds team to the Squadron
	 *	
	 *	@param team to be added
	 *	@return none
	 */
	void addTeam(Fireteam *team); 

	/** 
	 *  Removes team from Squadron 
	 *	
	 *	@param team to be removed
	 *	@return none
	 */
	void removeTeam(Fireteam *team); 

	/** 
	 *	Transfers up to \a number of units of \a type to \a target
	 *	
	 *	@param target Squadron to which the units will be transferred
	 *  @param type of unit to be transferred
	 *  @param number of units to be transferred
	 *	@return none
	 */
	void transferTo(Squadron *target, BWAPI::UnitType type, int number); 

	/** 
	 *	Transfers all units to Squadron \a target, leaving this Squadron empty
	 *	
	 *	@param target Squadron with which to merge
	 *	@return none
	 */
	void mergeWith(Squadron *target); 

	/** 
	 *	Adds a unit to the Squadron for allocation
	 *
	 *  @warning Do not add units to teams or squads through any other function. Doing so may cause errors.
	 *	
	 *	@param unit to be added
	 *	@return none
	 */
	void addUnit(BWAPI::Unit *unit); 


	/** 
	 *	Returns the total number of units of type typeToCount accross all Fireteams in squad
	 *	
	 *	@param typeToCount type of unit to be counted
	 *	@return Number of units of type typeToCount
	 */
	int getTotalOfType(BWAPI::UnitType typeToCount); 

	/** 
	 *	Returns the first non-full Fireteam of the type \a type
	 *	
	 *	@param type Fireteam 
	 *	@return The first non-full Fireteam of type \a type
	 *  @todo make this private?
	 */
	Fireteam *firstPartialTeam(BWAPI::UnitType type);
	
	/** 
	 *	Queries the state of the current ArmyTask
	 *	
	 *	@param none
	 *	@return True if the current ArmyTask is in progress
	 */
	bool taskInProgress();

	/** 
	 *	Queries the state of the current ArmyTask
	 *	
	 *	@param none
	 *	@return True if the current ArmyTask is complete
	 */
	bool taskComplete();

	/** 
	 *	Queries the state of the current ArmyTask
	 *	
	 *	@param none
	 *	@return True if the current ArmyTask has failed
	 */
	bool taskFailed();

	/** 
	 *	Returns the name of the current ArmyTask
	 *	
	 *	@param none
	 *	@return String representation of the ArmyTask's name
	 */
	std::string getTaskName(); 

	/** 
	 *	Issues this Squadron a new ArmyTask of type \a key
	 *	
	 *	@param key value representing the ArmyTask name
	 *	@return none
	 */
	void task(std::string key);	

	/** 
	 *	Orders the Squadron to execute the current ArmyTask
	 *	
	 *	@param none
	 *	@return none
	 */
	void doTask();

	/** 
	 *	Orders the Squadron to reposition its units according to current ArmyTask
	 *	
	 *	@param none
	 *	@return none
	 */
	void updateTeamMicro();

	/** 
	 *	The position at which the Squadron should execute its ArmyTask
	 *	
	 *	@param pos at which the Squadron should execute its ArmyTask
	 *	@return none
	 */
	void setObjectivePosition(BWAPI::Position pos);	

	/** 
	 *	Returns the position at which the Squadron has been ordered to execute its ArmyTask
	 *	
	 *	@param  none
	 *	@return the position at which the Squadron has been ordered to execute its ArmyTask
	 */
	BWAPI::Position getObjectivePosition() { return taskParameters.objectivePosition; }
};