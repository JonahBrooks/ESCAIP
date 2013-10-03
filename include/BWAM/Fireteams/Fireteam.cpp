#include <BWAM\Fireteams\Fireteam.h>
#include <BWAM\Fireteams\Specific\SpecificFireteams.h>
#include <vector>
#include <algorithm>
#include <math.h>

//class Fireteam {
//
//private:
//	UnitType coreType;	// The UnitType of all units within this Fireteam
//	int unitCount;		// Number of units in this team (TODO: use something smaller than an int)
//	int formation;		// Key for a specific layout/formation conditional on coreType 
//	int tactics;		// Key for specific strategy conditional on coreType
//	std::set<Unit*> units;	// Set of units within this team
//
//public:

Fireteam::Fireteam(BWAPI::UnitType initType) { 
	coreType = initType;
	unitCount = 0;
	mission.status = 0;
	mission.formation = 0;
	mission.tactics = 0;
	destroyed = false;
	formation.xSpacing = 20;
	formation.ySpacing = 20;
	setFormation(0); // Set initial formation to default
} 

// STATIC  ID numbers are taken from UnitType.cpp
Fireteam *Fireteam::createTeam(BWAPI::UnitType initType) {
	switch (initType.getID()) {
	case 0: // Terran_Marine
		return new Terran_Marine_Fireteam();
	case 1: // Terran_Ghost
		return new Terran_Ghost_Fireteam();
	case 2: // Terran_Vulure
		return new Terran_Vulture_Fireteam();
	case 3: // Terran_Goliath
		return new Terran_Goliath_Fireteam();
	case 5: // Terran_Siege_Tank_Tank_Mode
		return new Terran_Siege_Tank_Fireteam();
	case 8: // Terran_Wraith
		return new Terran_Wraith_Fireteam();
	case 9: // Terran_Science_Vessel
		return new Terran_Science_Vessel_Fireteam();
	case 11: // Terran_Dropship
		return new Terran_Dropship_Fireteam();
	case 12: // Terran_Battlecruiser
		return new Terran_Battlecruiser_Fireteam();
	case 32: // Terran_Firebat
		return new Terran_Firebat_Fireteam();	
	case 34: // Terran_Medic
		return new Terran_Medic_Fireteam();
	case 58: // Terran_Valkyrie
		return new Terran_Valkyrie_Fireteam();
	default:
		return NULL;
	}

}


// Returns a unit vector of format <x,y,suggested_magnitude> for a given unit
std::vector<double> Fireteam::getMovementVector(BWAPI::Unit *unit)
{	
	std::vector<double> toReturn(3,0);
	double relativeCharge = 0;
	double radius = 0;
	double maxRadius = enemyCharge.maxRadius;
	double xhat = 0;
	double yhat = 0;

	if(alliedCharge.maxRadius > maxRadius) maxRadius = alliedCharge.maxRadius;
	if(wallCharge.maxRadius > maxRadius) maxRadius = wallCharge.maxRadius;
	if(teamCharge.maxRadius > maxRadius) maxRadius = teamCharge.maxRadius;

	std::set<BWAPI::Unit*> nearbyUnits = unit->getUnitsInRadius((int)maxRadius);

	for(std::set<BWAPI::Unit*>::const_iterator i = nearbyUnits.begin(); i != nearbyUnits.end(); i++)
	{	relativeCharge = 0;
		radius = unit->getDistance((*i));
		if(radius == 0) continue; // Division by zero imminent
		if((*i)->getType().isBuilding()) continue; // Ignore buildings

		if((*i)->getPlayer() == unit->getPlayer())
		{

			if(units.count((*i)) == 1 && radius > teamCharge.minRadius && radius < teamCharge.maxRadius)
			{
				relativeCharge = (radius >= alliedCharge.toggleRadius) ? (-1 * alliedCharge.charge) : (alliedCharge.charge);
			}
			else if(radius > alliedCharge.minRadius && radius < alliedCharge.maxRadius)
			{
				relativeCharge = (radius >= alliedCharge.toggleRadius) ? (-1 * alliedCharge.charge) : (alliedCharge.charge);
			}
		}
		else if ((*i)->getPlayer()->isNeutral())
		{
			// TODO: walls and such
		}
		else if (unit->getPlayer()->isEnemy((*i)->getPlayer()))
		{ 
			if(radius > enemyCharge.minRadius && radius < enemyCharge.maxRadius)
			{
				relativeCharge = (radius >= enemyCharge.toggleRadius) ? (-1 * enemyCharge.charge) : (enemyCharge.charge);
			}
		}

		toReturn[0] += (((*i)->getPosition().x() - unit->getPosition().x())/radius) * (relativeCharge/(radius));
		toReturn[1] += (((*i)->getPosition().y() - unit->getPosition().y())/radius) * (relativeCharge/(radius));
	}

	toReturn[2] = sqrt(toReturn[0]*toReturn[0] + toReturn[1]*toReturn[1]);
	if(toReturn[2] > 0.001)
	{
		toReturn[2] = 100;
		toReturn[0] = toReturn[0] / toReturn[2];
		toReturn[1] = toReturn[1] / toReturn[2];
	}
	else
	{
		toReturn[0] = 0;
		toReturn[1] = 0;
		toReturn[2] = 0;
	}
	return toReturn;

}

void Fireteam::updateMicro()
{
	// Currently non-functional
	/*
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{	
		int kiteDelay = 6; // TODO: Specific to Marines. Specify this on a per team basis later.
		BWAPI::Broodwar->printf("Order Timer: %d", (*i)->getOrderTimer());
		BWAPI::Broodwar->printf("Order 1: %s", ((*i)->getOrder()) ? (*i)->getOrder().c_str() : "null");
		//BWAPI::Broodwar->printf("moveVector[2]: %f", moveVector[2]);
		//BWAPI::Broodwar->printf("KiteCooldown: %d", maxKiteCooldown);
		BWAPI::Broodwar->printf("Closest chokepoint is at (%d, %d)", BWTA::getNearestChokepoint((*i)->getPosition())->getSides().first.x(), BWTA::getNearestChokepoint((*i)->getPosition())->getSides().first.y());
		BWAPI::Broodwar->printf("Closest wall is at (%d, %d)", BWTA::getNearestUnwalkablePosition((*i)->getPosition()).x(), BWTA::getNearestUnwalkablePosition((*i)->getPosition()).y());
		BWAPI::Broodwar->printf("Order Target: %d", ((*i)->getOrderTarget()) ? (*i)->getOrderTarget()->getID() : 0);
		BWAPI::Broodwar->printf("Ground Cooldown: %d", (*i)->getGroundWeaponCooldown());
		BWAPI::Broodwar->printf("isAttackFrame() = %d", (*i)->isAttackFrame());
		BWAPI::Broodwar->printf("isStartingAttack() = %d", (*i)->isStartingAttack());
		BWAPI::Broodwar->printf("isAttacking() = %d", (*i)->isAttacking());
		BWAPI::Broodwar->printf("isMoving() = %d", (*i)->isMoving());
		BWAPI::Broodwar->printf("exists() = %d", (*i)->exists());
		
		//BWAPI::Broodwar->drawLine(BWAPI::CoordinateType::Map, (*i)->getPosition().x(), (*i)->getPosition().y(), BWTA::getNearestUnwalkablePosition((*i)->getPosition()).x(), BWTA::getNearestUnwalkablePosition((*i)->getPosition()).y(), BWAPI::Colors::Red);
		
		std::vector<double> moveVector = getMovementVector((*i));
		if(moveVector[2] > 0.001)
		{	
	//		BWAPI::Broodwar->printf("Moving unit by (%f,%f), toggleRadius: %f", moveVector[0]*moveVector[2], moveVector[1]*moveVector[2], enemyCharge.toggleRadius);
			moveVector[0] = moveVector[0]*moveVector[2] + (*i)->getPosition().x();
			moveVector[1] = moveVector[1]*moveVector[2] + (*i)->getPosition().y();
			bool allClear = true;
			bool inDanger = false;
			int minDist = 9999;
			for(std::set<BWAPI::Unit*>::const_iterator j = ((*i)->getUnitsInWeaponRange((*i)->getType().groundWeapon())).begin(); j != ((*i)->getUnitsInWeaponRange((*i)->getType().groundWeapon())).end(); j++)
			{
				if((*j)->getPlayer()->isEnemy((*i)->getPlayer()) && (*j)->getType().isBuilding() == false)
				{
					allClear = false;
					if((*i)->getDistance((*j)) < minDist)
					{
						minDist = (*i)->getDistance((*j));
					}
				}
				double dx = (*i)->getPosition().x() - ((*j)->getPosition().x() + (*j)->getType().topSpeed()*kiteDelay*cos((*j)->getAngle()));
				double dy = (*i)->getPosition().y() - ((*j)->getPosition().y() + (*j)->getType().topSpeed()*kiteDelay*sin((*j)->getAngle()));
				if(sqrt(dx*dx + dy*dy) <= BWAPI::UnitTypes::Protoss_Zealot.groundWeapon().maxRange())
				{	// TODO: Find a way to determine the enemies attack range instead of hard coding Zealot
					inDanger = true;
				}
			}
			if(allClear)
			{
				if((*i)->getDistance(mission.objectivePosition) > 75)
				{
					(*i)->attack(mission.objectivePosition,0);
				}
			}
			else
			{
				if((*i)->isUnderAttack())
				{
					inDanger = true;
				}
				if((*i)->getGroundWeaponCooldown() < 1 && minDist < enemyCharge.toggleRadius*1.1 && !inDanger)
				{
					(*i)->attack(BWAPI::Position((int)moveVector[0],(int)moveVector[1]),0);
				} 
				else
				{
					(*i)->move(BWAPI::Position((int)moveVector[0],(int)moveVector[1]),0);
				}
			}
		}
	}
	*/
}

bool Fireteam::isMember(BWAPI::Unit* unit) {
	bool toReturn = false;
	if (units.count(unit) > 0) {
		toReturn = true;
	}
	return toReturn;
}

void Fireteam::addUnit(BWAPI::Unit* unit) {
	units.insert(unit);
	
	FireteamFormationPosition *uPos = &formation.positionMap[unit];
	if(!formation.emptyPositions.empty()) {
		formation.positionMap[unit] = formation.emptyPositions.top();
		formation.emptyPositions.pop();
		uPos = &formation.positionMap[unit];
	} else {
		uPos->x = formation.placement[unitCount].first;
		uPos->y = formation.placement[unitCount].second;
	}
	uPos->xOffset = uPos->x * cos(formation.angle) * formation.xSpacing - uPos->y * sin(formation.angle) * formation.ySpacing;
	uPos->yOffset = uPos->x * sin(formation.angle) * formation.xSpacing + uPos->y * cos(formation.angle) * formation.ySpacing;

	unitCount++;

	if (mission.status >= 1 && (mission.fallBackPosition.x() != 0 || mission.fallBackPosition.y() != 0)) {
		unit->move(mission.fallBackPosition, false);
	}
	if (mission.status >= 2 && (mission.objectivePosition.x() != 0 || mission.objectivePosition.y() != 0)) {
		unit->attack(mission.objectivePosition, true); // TODO: Add a private function for deploying each unit
	}

} // Adds Unit* to the team

void Fireteam::removeUnit(BWAPI::Unit* unit) {
	if (isMember(unit)) {
		units.erase(unit);
		unitCount--;
		formation.emptyPositions.push(formation.positionMap[unit]);
		formation.positionMap.erase(unit);
		if(unitCount == 0) {
			destroyed = true;
		} else {
			destroyed = false;
		}
	} else {
		BWAPI::Broodwar->printf("Unit to be removed is not a member of this team.");
		BWAPI::Broodwar->pauseGame();
	}
}// Removes unit from team 

BWAPI::Unit *Fireteam::getFirstUnit()
{
	return (*(units.begin()));
}

bool Fireteam::isDestroyed()
{
	if(unitCount == 0) {
		destroyed = true;
	}
	return destroyed;
}

bool Fireteam::isInCombat()
{
	bool inCombat = false;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		if((*i)->exists() && (*i)->isAttacking() == true) {
			inCombat = true;
		}
	}
	return inCombat;
}


BWAPI::UnitType Fireteam::getUnitType() { return coreType; }
//int Fireteam::getUnitCount() { return unitCount; }


BWAPI::Position Fireteam::getPosition()
{
	std::vector<int> xs;
	std::vector<int> ys;
	
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		if((*i)->exists())
		{
			xs.push_back((*i)->getPosition().x());
			ys.push_back((*i)->getPosition().y());
		}
		else
		{
			BWAPI::Broodwar->printf("Unit no longer exists!");
			BWAPI::Broodwar->pauseGame();
			return BWAPI::Position(0,0);
		}
	}
	if(xs.size() > 0 && ys.size() > 0)
	{
		std::sort(xs.begin(), xs.end());
		std::sort(ys.begin(), ys.end());

		// TODO: Gather the median position of all units in the team?
		return BWAPI::Position(xs[xs.size()/2], ys[ys.size()/2]);
	}
	BWAPI::Broodwar->printf("No (%d) units in this Fireteam!", unitCount);
	return BWAPI::Position(0,0);
}


bool Fireteam::useAbility(std::string name)
{
	bool techUsed = true;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		if(name == "Unsiege")
		{
			(*i)->unsiege();
		}
		else if(name == "Decloak")
		{
			(*i)->decloak();
		}
		else
		{
			techUsed = (*i)->useTech(BWAPI::TechTypes::getTechType(name));
		}
	}

	return techUsed;
}

//** Uses tech on triggering unit or all units when an enemy is within radius
bool Fireteam::useAbilityOnSight(std::string name, int radius, bool includeBuildings, bool allUnits)
{
	bool techUsed = false;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		bool enemyFound = false;
		std::set<BWAPI::Unit*> unitsInRange = (*i)->getUnitsInRadius(radius);
		for(std::set<BWAPI::Unit*>::const_iterator j=unitsInRange.begin(); j != unitsInRange.end(); j++)
		{
			if((*j)->getPlayer()->isEnemy((*i)->getPlayer()) && ((*j)->getType().isBuilding() == false || includeBuildings))
			{
				enemyFound = true;
			}
		}
		if(enemyFound &&
			(name != "Personnel_Cloaking" || !(*i)->isCloaked()) &&
			(name != "Decloak" || (*i)->isCloaked()) &&
			(name != "Tank_Siege_Mode" || !(*i)->isSieged()) &&
			(name != "Unsiege" || (*i)->isSieged()) &&
			(name != "Stim_Packs" || !(*i)->isStimmed()) &&
			(name != "Defensive_Matrix" || !(*i)->isDefenseMatrixed())
			)
		{
			techUsed = true;
			if(allUnits)
			{
				techUsed = useAbility(name);
				break;
			}
			if(name == "Unsiege")
			{
				(*i)->unsiege();
			}
			else if(name == "Decloak")
			{
				(*i)->decloak();
			}
			else
			{
				techUsed = (*i)->useTech(BWAPI::TechTypes::getTechType(name));
			}
		}
	}

	return techUsed;
}

//** Uses tech on triggering unit or all units when no enemy is within radius
bool Fireteam::useAbilityOnClear(std::string name, int radius, bool includeBuildings, bool allUnits)
{
	bool techUsed = false;
	
	std::set<BWAPI::Unit*> unitsToUseTech;
	std::set<BWAPI::Unit*> unitsInRange;
	bool enemyFound = false;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		unitsInRange = (*i)->getUnitsInRadius(radius);
		for(std::set<BWAPI::Unit*>::const_iterator j=unitsInRange.begin(); j != unitsInRange.end(); j++)
		{
			if((*j)->getPlayer()->isEnemy((*i)->getPlayer()) && ((*j)->getType().isBuilding() == false || includeBuildings))
			{
				enemyFound = true;
			}
		}
		if(!enemyFound)
		{
			unitsToUseTech.insert((*i));
		}
	}
	if(allUnits && !unitsToUseTech.empty())
	{
			techUsed = useAbility(name);
	}
	else
	{
		for(std::set<BWAPI::Unit*>::const_iterator i=unitsToUseTech.begin(); i != unitsToUseTech.end(); i++)
		{
			if(	(name != "Personnel_Cloaking" || !(*i)->isCloaked()) &&
				(name != "Decloak" || (*i)->isCloaked()) &&
				(name != "Tank_Siege_Mode" || !(*i)->isSieged()) &&
				(name != "Unsiege" || (*i)->isSieged()) &&
				(name != "Stim_Packs" || !(*i)->isStimmed()) &&
				(name != "Defensive_Matrix" || !(*i)->isDefenseMatrixed())
				)
			{
				techUsed = true;
				if(name == "Unsiege")
				{
					(*i)->unsiege();
				}
				else if(name == "Decloak")
				{
					(*i)->decloak();
				}
				else
				{
					techUsed = (*i)->useTech(BWAPI::TechTypes::getTechType(name));
				}
			}
		}
	}

	return techUsed;
}

//** Uses tech on triggering unit or all units when a unit attacks an enemy
bool Fireteam::useAbilityOnAttack(std::string name, bool allUnits)
{
	bool techUsed = false;

	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		if((*i)->getGroundWeaponCooldown() > 0 &&
			(name != "Personnel_Cloaking" || !(*i)->isCloaked()) &&
			(name != "Decloak" || (*i)->isCloaked()) &&
			(name != "Tank_Siege_Mode" || !(*i)->isSieged()) &&
			(name != "Unsiege" || (*i)->isSieged()) &&
			(name != "Stim_Packs" || !(*i)->isStimmed()) &&
			(name != "Defensive_Matrix" || !(*i)->isDefenseMatrixed())
			)
		{
			techUsed = true;
			if(allUnits)
			{
				techUsed = useAbility(name);
				break;
			}
			if(name == "Unsiege")
			{
				(*i)->unsiege();
			}
			else if(name == "Decloak")
			{
				(*i)->decloak();
			}
			else
			{
				techUsed = (*i)->useTech(BWAPI::TechTypes::getTechType(name));
			}
		}
	}

	return techUsed;
}

//** Uses tech on triggering unit or all units when a unit is attacked
bool Fireteam::useAbilityOnAttacked(std::string name, bool allUnits)
{
	bool techUsed = false;

	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		if((*i)->isUnderAttack() &&
			(name != "Personnel_Cloaking" || !(*i)->isCloaked()) &&
			(name != "Decloak" || (*i)->isCloaked()) &&
			(name != "Tank_Siege_Mode" || !(*i)->isSieged()) &&
			(name != "Unsiege" || (*i)->isSieged()) &&
			(name != "Stim_Packs" || !(*i)->isStimmed()) &&
			(name != "Defensive_Matrix" || !(*i)->isDefenseMatrixed())
			)
		{
			techUsed = true;

			if(allUnits)
			{
				techUsed = useAbility(name);
				break;
			}
			if(name == "Unsiege")
			{
				(*i)->unsiege();
			}
			else if(name == "Decloak")
			{
				(*i)->decloak();
			}
			else
			{
				techUsed = (*i)->useTech(BWAPI::TechTypes::getTechType(name));
			}
		}
	}
	return techUsed;
}
	

void Fireteam::moveFormation(BWAPI::Position pos) {
	mission.objectivePosition = pos;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{	
		
		(*i)->move(BWAPI::Position::Position(pos.x() + (int)formation.positionMap[(*i)].xOffset, pos.y() + (int)formation.positionMap[(*i)].yOffset), 0);
	}
}


void Fireteam::move(BWAPI::Position pos) {
	mission.objectivePosition = pos;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		(*i)->move(pos, 0);
	}
	
}// Should be conditional on coreType


void Fireteam::deploy(BWAPI::Position pos) {
	//BWAPI::Broodwar->printf("Setting objectivePosition to coordinates (%d,%d)", pos.x(), pos.y());
	mission.objectivePosition = pos;
	if (mission.status == 0) {
	//	BWAPI::Broodwar->printf("Warning! Deploying team before critical mass! (%ss)", coreType.c_str());
	} else {
		mission.status = 2;
	}
	//int j = 0;
	for(std::set<BWAPI::Unit*>::const_iterator i=units.begin(); i != units.end(); i++)
	{
		if((*i)->isAttacking() == false) {
			(*i)->attack(pos, 0);
		}
	//	j++;	
	//	BWAPI::Broodwar->printf("Unit %d of %d has been deployed.", j, unitCount);
	}
	

}// Should be conditional on coreType


void Fireteam::assist(Fireteam* toAssist) { // Should be conditional on coreType and toAssist.getUnitType()
	if(toAssist->getUnitCount() == 0)
	{
		return;
	}

	std::set<BWAPI::Unit*>::const_iterator target = toAssist->units.begin();

	for(std::set<BWAPI::Unit*>::const_iterator i = units.begin(); i != units.end(); i++)
	{
		(*i)->follow((*target));
		if((++target) == toAssist->units.end())
		{
			target = toAssist->units.begin();
		}
	}
}
	//
void Fireteam::setFormation(int key) {
	if(key == 0)
	{
		int j = 0;
		for(int i = 0; i < 200; ++i) {
			formation.placement[i].first = j; formation.placement[i].second = 0;
			if(j <= 0){
				j = -(j-1);
			} else {
				j = -j;
			}
		}
	}
}	// Conditional on coreType

void Fireteam::setTactics(int) {}	// Conditional on coreType
//	int Fireteam::getFormation(int);
//	int Fireteam::getTactics(int);	
