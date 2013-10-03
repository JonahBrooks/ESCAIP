#pragma once
#include <iostream>
#include <map>
#include <string>
#include <BWAPI.h>
#include <BWAM\Fireteams\Fireteam.h>


/** 
*	@struct ArmyTaskContext
*	
*	Stores the state of the ArmyTask
*	
*/
struct ArmyTaskContext {
	std::string taskName;
	BWAPI::Position objectivePosition;
	std::set<Fireteam*> *teamSet;
	std::set<Fireteam*> attackTeams;
	std::set<Fireteam*> supportTeams;
	std::set<Fireteam*> siegeTeams;
	bool taskFailed;
	bool taskInProgress;
	bool taskComplete;
};

 /** @file ArmyTask.h */

class  ArmyTask {
	public:
		static std::map<std::string,ArmyTask*> taskMap; /**< Maps the SpecificArmyTasks Singletons to their string representations */
	   /** 
		*	Returns a singleton object of this ArmyTask
		*	
		*	@param none
		*	@return Returns a singleton object of this ArmyTask
		*/
		static ArmyTask& init_template() {				
			static ArmyTask singleton("");
			return singleton; 
		}

	   /** 
		*	Issues the order to complete the assigned task
		*	
		*	@param none
		*	@return none
		*/
		virtual void doTask(ArmyTaskContext*) { }

	   /*
	    *	Checks conditions for taskFailed and updates the ArmyTaskContext argument
		*	 
		*	@param A pointer to an ArmyTaskContext object that stores the contexts for this task
		*	@return none
		*/
		virtual void updateTaskFailed(ArmyTaskContext*);

	   /*
	    *	Checks conditions for taskInProgress and updates the ArmyTaskContext argument
		*	 
		*	@param A pointer to an ArmyTaskContext object that stores the contexts for this task
		*	@return none
		*/
		virtual void updateTaskInProgress(ArmyTaskContext*);
		
	   /*
	    *	Checks conditions for taskComplete and updates the ArmyTaskContext argument
		*	 
		*	@param A pointer to an ArmyTaskContext object that stores the contexts for this task
		*	@return none
		*/
		virtual void updateTaskComplete(ArmyTaskContext*);

	   /** 
		*	Updates the task progress fields of the ArmyTaskContext argument
		*	
		*	@param A pointer to an ArmyTaskContext object that stores the contexts for this task
		*	@return none
		*/
		virtual void updateTaskProgress(ArmyTaskContext*);
		
	   /** 
		*	Assigns teams as this tactic sees fit regarding which roles each team should play
		*	
		*	@param A pointer to an ArmyTaskContext object that stores the contexts for this task
		*	@return none
		*/
		virtual void adjustTeams(ArmyTaskContext*) { }
		
	   /** 
		*	Give microing orders to each team as this tactic sees fit
		*	
		*	@param A pointer to an ArmyTaskContext object that stores the contexts for this task
		*	@return none
		*/
		virtual void updateTeamMicro(ArmyTaskContext*);



	protected:
	   /** 
		*	Creates this ArmyTask singleton and adds it to taskMap
		*	
		*	@param The string representation (name) to be used to identify this ArmyTask
		*	@return none
		*	@note	Constructors and Destructors are protected to ensure there is only one instance (singleton) of this class. 
		*/
		ArmyTask(std::string key) {
			ArmyTask::taskMap.insert(std::make_pair(key, this));
		}
		ArmyTask () { }
		~ArmyTask () { }
};