#pragma once
#include <BWAM\ArmyTasks\ArmyTask.h>

class Attack: public ArmyTask {
	public:
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		static Attack& init() {
			static Attack singleton("Attack");
			return singleton;
		}

		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		void doTask(ArmyTaskContext*);

		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTaskProgress(ArmyTaskContext*);

		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void adjustTeams(ArmyTaskContext*);

		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTeamMicro(ArmyTaskContext*);
	private:
		Attack(std::string key) : ArmyTask(key) { }
		Attack() { }
		~Attack() { }
};

class MoveOut: public ArmyTask {
	public:

		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		static MoveOut& init() {
			static MoveOut singleton("MoveOut");
			return singleton;
		}
		
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		void doTask(ArmyTaskContext*);
		
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTaskProgress(ArmyTaskContext*);
		
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void adjustTeams(ArmyTaskContext*);
		
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTeamMicro(ArmyTaskContext*);
	private:
		MoveOut(std::string key) : ArmyTask(key) { }
		MoveOut() { }
		~MoveOut() { }
};

class Reinforce: public ArmyTask {
	public:
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		static Reinforce& init() {
			static Reinforce singleton("Reinforce");
			return singleton;
		}
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		void doTask(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTaskProgress(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void adjustTeams(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTeamMicro(ArmyTaskContext*);
	private:
		Reinforce(std::string key) : ArmyTask(key) { }
		Reinforce() { }
		~Reinforce() { }
};

class Rush: public ArmyTask {
	public:
		static Rush& init() {
			static Rush singleton("Rush");
			return singleton;
		}
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		void doTask(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTaskProgress(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void adjustTeams(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTeamMicro(ArmyTaskContext*);
	private:
		Rush(std::string key) : ArmyTask(key) { }
		Rush() { }
		~Rush() { }
};

class Idle: public ArmyTask {
	public:

		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		static Idle& init() {
			static Idle singleton("Idle");
			return singleton;
		}
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		void doTask(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTaskProgress(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void adjustTeams(ArmyTaskContext*);
			
		/** 
		*	Generic Comment
		*	
		*	@param none
		*	@return none
		*/
		virtual void updateTeamMicro(ArmyTaskContext*);
	private:
		Idle(std::string key) : ArmyTask(key) { }
		Idle() { }
		~Idle() { }
};
