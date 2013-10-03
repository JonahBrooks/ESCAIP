#pragma once
#include <set>
#include <map>
#include <string>
#include <stack>
#include <vector>
#include <BWAPI.h>
#include <BWTA.h>

 /** @file Fireteam.h
  *   
  */

class Fireteam {

protected:
	/** @struct FireteamFormationPosition
	 *
	 *  Stores information needed to properly arrange FireteamFormations at a specific position
	 *
	 *  @todo	Finish implementing FireteamFormationPosition
	 *	@note	Formations are not finished - this may change
	 */
	struct FireteamFormationPosition {
		int x;
		int y;
		double xOffset;
		double yOffset;
	};

	/** @struct FireteamFormation
	 *
	 *  Stores information needed to properly arrange FireteamFormations
	 *
	 *  @todo	Finish implementing FireteamFormation
	 *	@note	Formations are not finished - this may change
	 */
	struct FireteamFormation {
		BWAPI::Position center;
		double angle;	/**< Direction the Formation is "facing," in radians CLOCKWISE from east */
		int xSpacing;	/**< Distance between units along the x axis */
		int ySpacing;	/**< Distance between units along the y axis */
		std::map<BWAPI::Unit*,FireteamFormationPosition> positionMap;
		std::stack<FireteamFormationPosition> emptyPositions;
		std::pair<int,int> placement[200];	/**< The x,y position of the nth unit to be placed */
	} formation;

	/** @struct DeploymentOrders
	 *
	 *  Stores information on the current status of the FireTeams objective
	 *
	 *  @todo Double check whether or not fallBackPosition is needed
	 */
	struct DeploymentOrders {
		BWAPI::Position objectivePosition;	/**< Position of the team's target objective */ 
		BWAPI::Position fallBackPosition;	/**< Position to where the team should retreat when needed */
		
		int status;			/**< Key for the current status of the Fireteam. */
		int formation;		/**< Key for a specific layout/formation conditional on coreType */
		int tactics;		/**< Key for specific strategy conditional on coreType */
	} mission;

	BWAPI::UnitType coreType;	/**< The UnitType of all units within this Fireteam */
	int unitCount;			    /**< Number of units in this team */
	bool destroyed;
	
	Fireteam(BWAPI::UnitType initType); 

public:
	/** @struct movementCharges
	 *
	 *  Stores information on how units should interact with each other through the UnitMicro code
	 *
	 *  @todo This is part of the UnitMicro code that doesn't work
	 */
	struct movementCharges {
		double charge;
		double minRadius;
		double toggleRadius;
		double maxRadius;
	} alliedCharge, enemyCharge, wallCharge, teamCharge;

	static Fireteam *createTeam(BWAPI::UnitType initType);

	std::set<BWAPI::Unit*> units;	// Set of units within this team

	
   /** 
	*	Calculates the direction in which \a unit should move based on \a alliedCharge, \a enemyCharge, \a wallCharge, and \a teamCharge
	*	
	*	@param  The \a unit for which a new movement vector should be calculated
	*	@return A unit std::vector of format <x,y,suggested_magnitude> for \a unit
	*	@todo   Unit Micro currently nonfuntional - requires work
	*/
	std::vector<double> getMovementVector(BWAPI::Unit *unit);

   /** 
	*	Issues new movement orders for each unit in the Fireteam based on the results of getMovementVector()
	*	
	*	@see    getMovementVector
	*	@param  none
	*	@return none
	*	@todo   Unit Micro currently nonfunctional - requires works
	*/
	virtual void updateMicro();

   /** 
	*	Checks to see if \a unit is a member of this Fireteam 
	*	
	*	@param	The \a unit that is being checked 
	*	@return True if \a unit is a memeber of this Fireteam
	*/
	virtual bool isMember(BWAPI::Unit *unit); 

   /** 
	*	Adds \a unit to this Fireteam
	*	
	*	@param The \a unit to be added
	*	@return none
	*/
	virtual void addUnit(BWAPI::Unit *unit);  

   /** 
	*	Removes unit from this Fireteam
	*	
	*	@param The \a unit to be removed
	*	@return none
	*/
	virtual void removeUnit(BWAPI::Unit *unit); 

   /** 
	*	Finds and returns the first BWAPI::Unit in this Fireteam
	*	
	*	@param none
	*	@return A pointer to the first BWAPI::Unit in this Fireteam
	*/
	virtual BWAPI::Unit *getFirstUnit();

   /** 
	*	Checks if this Fireteam is destroyed
	*	
	*	@param	none
	*	@return True is this Fireteam is destroyed
	*	@todo	Check to see if this is necessary 
	*/
	virtual bool isDestroyed();

   /** 
	*	Checks if this Fireteam is in combat
	*	
	*	@param	none
	*	@return True if any member of the team is currently attacking
	*/
	virtual bool isInCombat();

   /** 
	*	Returns the BWAPI::UnitType contained in this Fireteam
	*	
	*	@param none
	*	@return The BWAPI::UnitType contained in this Fireteam
	*/
	virtual BWAPI::UnitType getUnitType();

   /** 
	*	Returns current number of units in this Fireteam
	*	
	*	@param none
	*	@return The current number of units in this Fireteam
	*/
	virtual int getUnitCount() { return unitCount; }

   /**   
	*	Returns BWAPI::Position of this Fireteam
	*	
	*	@param none
	*	@return none
	*	@warning The exact meaning of a Fireteams position is currently undefined
	*	@todo	The position of a Fireteam is currently undefined
	*/
	virtual BWAPI::Position getPosition();

   /**   
	*	Orders all units in this Fireteam to use the ability named \a name
	*	
	*	@see	BWAPI::TechType
	*	@param	The \a name of ability to be used
	*	@return True if the ability was successfully used
	*/
	virtual bool useAbility(std::string name);

   /**   
	*	If one or more unit in this Fireteam is within \a radius of an enemy unit (or enemy building if \a includeBuildings is true)
	*	order the all qualifying unit (or all units in this Fireteam if \a allUnits is true) to use the ability named \a name
	*
	*	@param The \a name of the ability to be used, the \a radius in which the enemy unit must be to be considere "onsight", \a includeBuildings if true enemy buildings are within radius include if false exclude buildings, and \a allUnits if true orders all units to use ability \a name and if false orders only all qualified unit to use ability
	*	@return none
	*/
	bool useAbilityOnSight(std::string name, int radius, bool includeBuildings, bool allUnits); 

   /**   
	*	Trigger only when useAbilityOnSight does not (inverse of useAbilityOnSight)
	*	Uses ablity only when enemies are NOT in radius
	*
	*	@see	useAbilityOnSight
	*	@param	none
	*	@return none
	*/
	bool useAbilityOnClear(std::string name, int radius, bool includeBuildings, bool allUnits);

   /**   
	*	If any unit in this Fireteam is currently attaching order all units (if allUnits = true)
	*	Or all attaching units (if allUnits = false) to use ability \a name
	*	
	*	@param	The \a name of the ability to be used, and \a allUnits if true orders all units to use ability \a name and if false orders only all qualified unit to use ability
	*	@return If at least one unit used an ability
	*/
	bool useAbilityOnAttack(std::string name, bool allUnits);

   /**   
	*	If any unit in this Fireteam is currently being attached order all units (if allUnits = true)
	*	Or all attached units (if allUnits = false) to use ability \a name
	*	
	*	@param	The \a name of the ability to be used, and \a allUnits if true orders all units to use ability \a name and if false orders only all qualified unit to use ability
	*	@return If at least one unit used an ability
	*/
	bool useAbilityOnAttacked(std::string name, bool allUnits);
	
   /** 
	*	Orders Fireteam to move in formation to \a pos
	*	
	*	@param The \a pos to which the Fireteams should move
	*	@return none
	*	@todo	Return false if pos is not valid
	*/
	virtual void moveFormation(BWAPI::Position pos);
   /** 
	*	Orders Fireteam to move to \a pos ignoring formation
	*	
	*	@param The \a pos to which the Fireteams should move
	*	@return none
	*	@todo	Return false if pos is not valid
	*	@todo	Possibly merge with moveFormation adding another param to distinguish between the two
	*/

	virtual void move(BWAPI::Position pos);	

   /** 
	*	Orders all units within the Fireteam to attach at position \a pos
	*	
	*	@param The \a pos to which the Fireteam should move
	*	@return none
	*	@note	This is the default funtion to be overwriten upon inheritence 
	*	@see	SpecificFireteam.h
	*/
	virtual void deploy(BWAPI::Position pos); 

   /** 
	*	Orders each unit in this Fireteam to follow one unit in the Fireteam \a toAssist
	*	
	*	@param	\a toAssist is the Fireteam to be assited
	*	@return none
	*	@note	This is the default funtion to be overwriten upon inheritence 
	*	@see	SpecificFireteam.h
	*/
	virtual void assist(Fireteam* toAssist); 

   /** 
	*	Sets the current FireteamFormation for this Fireteam
	*	
	*	@param	A key to determine which formation is to be used 
	*	@return none
	*	@todo	Not complete
	*/
	virtual void setFormation(int);	

   /** 
	*	Generic Comment
	*	
	*	@param none
	*	@return none
	*	@todo	This is not used - Possibly obsolete?
	*/
	virtual void setTactics(int);	


};