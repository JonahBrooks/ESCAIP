#pragma once

#include <BWAM\ArmyModule.h>

std::map<AM::AID, Squadron*> ArmyModule::armyMap;

// Initializes the ArmyModule, including each SpecificArmyTask singleton 
void ArmyModule::init() {
	Rush::init();
	Idle::init();
	MoveOut::init();
	Reinforce::init();
	Attack::init();
}

// Creates a new squadron, returns the id
//// TODO: Should this be "createArmy" or something for abstract things and such?
AM::AID ArmyModule::createSquad() {
	Squadron *newSquad = new Squadron();
	armyMap.insert(std::pair<AM::AID, Squadron*>(newSquad,newSquad));
	return newSquad;
}

// Adds unit to list 
//// Do we need this?
void ArmyModule::decommissionUnit(BWAPI::Unit* unit) {

	Squadron::decommissionUnit(unit);
}

// Transfers up to int number of UnitType units to Squadron*
void ArmyModule::transferTo(AM::AID idFrom, AM::AID idTo, BWAPI::UnitType unitType, int num) {

	armyMap[idFrom]->transferTo(armyMap[idTo], unitType, num);
}

// Transfers all units to Squadron*, leaving this Squadron empty
void ArmyModule::mergeWith(AM::AID idFrom, AM::AID idTo) {

	if(armyMap[idFrom] && armyMap[idTo]) 
	{
		armyMap[idFrom]->mergeWith(armyMap[idTo]);
	}
	else
	{
		BWAPI::Broodwar->printf("No squad matching the given ID");
	}
		//// TODO: Do we need to now deleteSquad?
}

// Adds a unit to a squadron for allocation
void ArmyModule::addUnit(AM::AID id, BWAPI::Unit* unit) {

	if (armyMap[id]) {
		armyMap[id]->addUnit(unit);
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
	}
}

// Returns the total number of units of given type accross all Fireteams in squad
int ArmyModule::getTotalOfType(AM::AID id, BWAPI::UnitType unitType) {
	
	if(armyMap[id]) {
		return armyMap[id]->getTotalOfType(unitType);
	} 
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
		return 0;
	}


}

//// TODO: Change these to one query returning a struct of information?
// Queries for the status of a squadron's task
bool ArmyModule::taskInProgress(AM::AID id) {

	if(armyMap[id]) {
		return armyMap[id]->taskInProgress();
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
		return false;
	}
}

bool ArmyModule::taskComplete(AM::AID id) {
	if (armyMap[id]) {
		return armyMap[id]->taskComplete();
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
		return false;
	}
}

bool ArmyModule::taskFailed(AM::AID id) {

	if (armyMap[id]) {
		return armyMap[id]->taskFailed();
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
		return false;
	}
}

// Returns the name of the given Squadron's current task
std::string ArmyModule::getTaskName(AM::AID id) {
	
	if (armyMap[id]) {
		return armyMap[id]->getTaskName();
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
		return "";
	}
}

// Gives the given Squadron a new task
void ArmyModule::task(AM::AID id, std::string taskName) {

	if (armyMap[id]) {
		armyMap[id]->task(taskName);
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
	}
}

// Orders the given Squadron to perform its task
void ArmyModule::doTask(AM::AID id) {
	
	if (armyMap[id]) {
		armyMap[id]->doTask();
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
	}
}

// Requests that the given squadron updates its unit positions
void ArmyModule::updateTeamMicro(AM::AID id) {
	
	if (armyMap[id]) {
		armyMap[id]->updateTeamMicro();
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
	}
}

// Sets the position to which the given Squadron sould perform its task
void ArmyModule::setObjectivePosition(AM::AID id, BWAPI::Position pos) {
	
	if (armyMap[id]) {
		armyMap[id]->setObjectivePosition(pos);
	}
	else {
		BWAPI::Broodwar->printf("No squad matching the given ID");
	}
}