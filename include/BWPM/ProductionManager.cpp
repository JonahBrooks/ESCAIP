#include "ProductionManager.h"

//// Undefining these at the bottom of the page. These really shouldn't exist at all.....
#define incomePerMiner .038
#define incomePerRefiner .073
#define dependencyPriority 60
#define expansionPriority 58
#define unitPriority 55
#define productionPriority 40
#define supplyRatio .7
#define EPSILON_INCOME (1.0/120.0)

ProductionManager::ProductionManager(void)
{
	taskMan = new TaskManager("Build.txt");
	workMan = new WorkerManager();
	switchingOrdersWhenReady = false;
	buildCurrentOrder = false;
	//firstBuild = false;
	frameCount = 5000;
	acceptableMineralLosses = 0;
	acceptableGasLosses = 0;
	gasCostPerFrame = 0;
	mineralCostPerFrame = 0;

	supplyLimit = 30;
	whichToBuild = BWAPI::UnitTypes::None;
	barracksCount = taskMan->getInactiveUnits(UnitTypes::Terran_Barracks);
	barracks = new ProductionOrder();
	tempBarracks = new ProductionOrder();
	factoryCount = taskMan->getInactiveUnits(UnitTypes::Terran_Factory);
	machineShopCount = taskMan->getInactiveUnits(UnitTypes::Terran_Machine_Shop);
	factory = new ProductionOrder();
	tempFactory = new ProductionOrder();
	starportCount = taskMan->getInactiveUnits(UnitTypes::Terran_Starport);
	controlTowerCount = taskMan->getInactiveUnits(UnitTypes::Terran_Control_Tower);
	starport = new ProductionOrder();
	tempStarport = new ProductionOrder();
}


ProductionManager::~ProductionManager(void)
{
}

bool ProductionManager::exists(UnitType unit)
{
	set<UnitType> pendUnits = taskMan->getPendingTasks();
	if(pendUnits.find(unit) != pendUnits.end())
		return true;
	for(unsigned int c = 0; c < buildings.size(); c++)
		if(buildings[c]->getType() == unit)
			return true;
	if(unit == UnitTypes::Unknown || unit == UnitTypes::None)
		return true;

	return false;
}

bool ProductionManager::actuallyExists(UnitType unit)
{
	for(unsigned int c = 0; c < buildings.size(); c++)
		if(buildings[c]->getType() == unit)
			return true;
	if(unit == UnitTypes::Unknown || unit == UnitTypes::None)
		return true;

	return false;
}

void ProductionManager::setIncomes()
{
	mineralIncomePerFrame = workMan->getAdjustedMinerCount() * (float)incomePerMiner;
	gasIncomePerFrame = workMan->getAdjustedRefinerCount() * (float)incomePerRefiner;
}

void ProductionManager::recalculateSupply()
{
	//Broodwar->printf("%d",taskMan->getInactiveUnits(UnitTypes::Terran_Supply_Depot));
	//Broodwar->printf("supply depots: %d", getNumSupplyDepots());
	supplyLimit = taskMan->getInactiveUnits(UnitTypes::Terran_Supply_Depot) * 8 + getNumSupplyDepots() * 8;
}

void ProductionManager::manage() ////////////////////big one here
{
	if(Broodwar->self()->supplyUsed() > Broodwar->self()->supplyTotal() * supplyRatio)
	{
		recalculateSupply();
		if(Broodwar->self()->supplyUsed() / 2 > supplyLimit * supplyRatio)
		{
			//Broodwar->printf("%d, %d", Broodwar->self()->supplyUsed() / 2, supplyLimit);
			taskMan->addUnitTask(UnitTypes::Terran_Supply_Depot,1);
			recalculateSupply();
		}
	}

	doAddons();

	doMaintenanceProduction();
	
	//BWAPI::Broodwar->printf("Reftasks: %d:%d", taskMan->getInactiveUnits(UnitTypes::Terran_Refinery),taskMan->getActiveUnits(UnitTypes::Terran_Refinery));
		
	if(gasCostPerFrame > EPSILON_INCOME 
		&& workMan->getNewGasPosition().x() != 0 
		&& taskMan->getInactiveUnits(UnitTypes::Terran_Refinery) == 0
		&& taskMan->getActiveUnits(UnitTypes::Terran_Refinery) == 0)
	{
		BWAPI::TilePosition gasPos = workMan->getNewGasPosition();
		taskMan->addUnitTask(UnitTypes::Terran_Refinery, workMan->getNewGasPosition());
	}



	if(buildCurrentOrder)
	{
		//doProduction();
		if(!switchingOrdersWhenReady)
		{
			buildMoreProduction(false);

		}
	}
	if(switchingOrdersWhenReady)
	{
		buildMoreProduction(true);
		//clearDependencies();
		//resolveDependencies(true);
		//buildDependencies();

		for(unsigned int c = 0; c < buildings.size(); c++)
		{
			if(switchUnits.find(buildings[c]->getType()) != switchUnits.end() && buildings[c]->getRemainingBuildTime() < 100)
				switchUnits.erase(buildings[c]->getType());
		}
		if(switchUnits.size() == 0)
		{
			//Broodwar->printf("run it now?");
			runOrderNow();
		}
	}
}

void ProductionManager::buildMoreProduction(bool temp)
{
	setTimeAndCosts(temp);
	//Broodwar->printf("income: %f, %f. cost: %f, %f. time: %d", mineralIncomePerFrame, gasIncomePerFrame, mineralCostPerFrame, gasCostPerFrame, timeToProduce);

	if(mineralCostPerFrame < mineralIncomePerFrame + acceptableMineralLosses && gasCostPerFrame < gasIncomePerFrame + acceptableGasLosses)
	{
		if(whichToBuild == BWAPI::UnitTypes::Terran_Barracks)
		{
			//taskMan->addUnitTask(UnitTypes::Terran_Barracks, 1);
			barracksCount += 1;
			setTimeAndCosts(temp);
			if(mineralCostPerFrame < mineralIncomePerFrame && gasCostPerFrame < gasIncomePerFrame)
				taskMan->addUnitTask(UnitTypes::Terran_Barracks, 1, productionPriority);
			else
				barracksCount -= 1;
		}
		if(whichToBuild == BWAPI::UnitTypes::Terran_Factory)
		{
			//taskMan->addUnitTask(UnitTypes::Terran_Factory, 1);
			factoryCount += 1;
			setTimeAndCosts(temp);
			if(mineralCostPerFrame < mineralIncomePerFrame && gasCostPerFrame < gasIncomePerFrame)
				taskMan->addUnitTask(UnitTypes::Terran_Factory, 1, productionPriority);
			else
				factoryCount -= 1;
		}
		if(whichToBuild == BWAPI::UnitTypes::Terran_Machine_Shop)
		{
			machineShopCount += 1;
			setTimeAndCosts(temp);
			if(mineralCostPerFrame < mineralIncomePerFrame && gasCostPerFrame < gasIncomePerFrame)
				taskMan->addUnitTask(UnitTypes::Terran_Machine_Shop, 1, productionPriority);
			else
				machineShopCount -= 1;
		}
		if(whichToBuild == BWAPI::UnitTypes::Terran_Starport)
		{
			//taskMan->addUnitTask(UnitTypes::Terran_Starport, 1);
			starportCount += 1;
			setTimeAndCosts(temp);
			if(mineralCostPerFrame < mineralIncomePerFrame && gasCostPerFrame < gasIncomePerFrame)
				taskMan->addUnitTask(UnitTypes::Terran_Starport, 1, productionPriority);
			else
				starportCount -= 1;
		}
		if(whichToBuild == BWAPI::UnitTypes::Terran_Control_Tower)
		{
			controlTowerCount += 1;
			setTimeAndCosts(temp);
			if(mineralCostPerFrame < mineralIncomePerFrame && gasCostPerFrame < gasIncomePerFrame)
				taskMan->addUnitTask(UnitTypes::Terran_Control_Tower, 1, productionPriority);
			else
				controlTowerCount -= 1;
		}
	}
	setTimeAndCosts(temp);
}

void ProductionManager::resolveDependencies(bool temp)
{
	eta = 0;
	int tempEta = 0, deltaEta = 0;
	vector<UnitType> units;
	map<UnitType, int> *map1, *map2, *map3;
	if(temp)
	{
		map1 = tempBarracks->getUnits();
		map2 = tempFactory->getUnits();
		map3 = tempStarport->getUnits();
	}
	else
	{
		map1 = barracks->getUnits();
		map2 = factory->getUnits();
		map3 = starport->getUnits();
	}
	map<UnitType, int>::iterator c;
	for(c = map1->begin(); c != map1->end(); c++)
		units.push_back((*c).first);
	for(c = map2->begin(); c != map2->end(); c++)
		units.push_back((*c).first);
	for(c = map3->begin(); c != map3->end(); c++)
		units.push_back((*c).first);
	for(c = maintenanceMap.begin(); c != maintenanceMap.end(); c++)
		units.push_back((*c).first);
	for(unsigned int i = 0; i < units.size(); i++)
	{
		deltaEta = resolveUnitDependency(units[i]);
		if(deltaEta > tempEta)
			tempEta = deltaEta;
	}
	eta += tempEta;
}

int ProductionManager::resolveUnitDependency(UnitType unit)
{
	int tempEta = 0, deltaEta = 0;
	map<UnitType, int> reqs = unit.requiredUnits();
	map<UnitType, int>::iterator c;
	for(c = reqs.begin(); c != reqs.end(); c++)
	{
		if(!actuallyExists((*c).first) && (*c).first != UnitTypes::Terran_SCV && (*c).first != UnitTypes::Terran_Command_Center)
		{
			//Broodwar->printf("%s",(*c).first.getName().c_str());
			requiredUnits.insert((*c).first);
			deltaEta = (*c).first.buildTime();
			deltaEta += resolveUnitDependency((*c).first);
			if(deltaEta > tempEta)
				tempEta = deltaEta;
		}
	}
	
	return tempEta;
}

void ProductionManager::buildDependencies()
{
	set<UnitType>::iterator c;
	for(c = requiredUnits.begin(); c != requiredUnits.end(); c++)
	{
		if(!exists(*c))
		{
			taskMan->addUnitTask((*c),1,dependencyPriority);
			if((*c) == UnitTypes::Terran_Barracks)
				barracksCount++;
			if((*c) == UnitTypes::Terran_Factory)
				factoryCount++;
			if((*c) == UnitTypes::Terran_Machine_Shop)
				machineShopCount++;
			if((*c) == UnitTypes::Terran_Starport)
				starportCount++;
			if((*c) == UnitTypes::Terran_Control_Tower)
				controlTowerCount++;
		}
	}
}

void ProductionManager::clearDependencies()
{
	eta = 0;
	minsReq = 0;
	gasReq = 0;
	requiredUnits.clear();
}

void ProductionManager::getDependencyCosts()
{
	set<UnitType>::iterator c;
	for(c = requiredUnits.begin(); c != requiredUnits.end(); c++)
	{
		minsReq += (*c).mineralPrice();
		gasReq += (*c).gasPrice();
	}
}

void ProductionManager::setTimeAndCosts(bool temp)
{
	timeToProduce = getCurrentProductionTime(temp);
	mineralCostPerFrame = getTotalMineralCost(temp) / (float)timeToProduce;
	gasCostPerFrame = getTotalGasCost(temp) / (float)timeToProduce;
}

void ProductionManager::produce(map<UnitType, int>::iterator c, bool &built)
{
	int amt = taskMan->getActiveUnits((*c).first);
	//Broodwar->printf("%s: %d",(*c).first.getName().c_str(),amt);
	if(amt < (*c).second)
	{
		built = false;
		if(amt + taskMan->getInactiveUnits((*c).first) < (*c).second)
			taskMan->addUnitTask((*c).first, (*c).second - amt - taskMan->getInactiveUnits((*c).first), unitPriority);
	}
}

void ProductionManager::doProduction()
{
	bool built = true;
	map<UnitType, int>::iterator c;
	for(c = barracks->getUnits()->begin(); c != barracks->getUnits()->end(); c++)
	{
		produce(c, built);
	}
	for(c = factory->getUnits()->begin(); c != factory->getUnits()->end(); c++)
	{
		produce(c, built);
	}
	for(c = starport->getUnits()->begin(); c != starport->getUnits()->end(); c++)
	{
		produce(c, built);
	}
	
	if(built)
	{
		for(c = barracks->getUnits()->begin(); c != barracks->getUnits()->end(); c++)
		{
			taskMan->clearFromActiveMap((*c).first,(*c).second);
		}
		for(c = factory->getUnits()->begin(); c != factory->getUnits()->end(); c++)
		{
			taskMan->clearFromActiveMap((*c).first,(*c).second);
		}
		for(c = starport->getUnits()->begin(); c != starport->getUnits()->end(); c++)
		{
			taskMan->clearFromActiveMap((*c).first,(*c).second);
		}
	}
}

void ProductionManager::doMaintenanceProduction()
{
	map<UnitType, int>::iterator c;
	for(c = maintenanceMap.begin(); c != maintenanceMap.end(); c++)
	{
		if(Broodwar->self()->allUnitCount((*c).first) + taskMan->getInactiveUnits((*c).first) < (*c).second)
			taskMan->addUnitTask((*c).first,1);
	}
}

void ProductionManager::doAddons()
{
	set<UnitType> pend = taskMan->getPendingTasks();
	if(requiredUnits.find(UnitTypes::Terran_Machine_Shop) != requiredUnits.end() && pend.find(UnitTypes::Terran_Machine_Shop) == pend.end())
		taskMan->addUnitTask(UnitTypes::Terran_Machine_Shop, 0, 1);
	if(requiredUnits.find(UnitTypes::Terran_Control_Tower) != requiredUnits.end() && pend.find(UnitTypes::Terran_Control_Tower) == pend.end())
		taskMan->addUnitTask(UnitTypes::Terran_Control_Tower, 0, 1);
	if(requiredUnits.find(UnitTypes::Terran_Physics_Lab) != requiredUnits.end() && pend.find(UnitTypes::Terran_Physics_Lab) == pend.end())
		taskMan->addUnitTask(UnitTypes::Terran_Physics_Lab, 0, 1);
}

int ProductionManager::getCurrentProductionTime(bool temp)
{
	whichToBuild = BWAPI::UnitTypes::None;
	int tempTime, bigTime = 0;
	if(barracksCount != 0)
	{
		if(temp)
			tempTime = tempBarracks->getProductionTime(BWAPI::UnitTypes::Terran_Barracks) / barracksCount;
		else
			tempTime = barracks->getProductionTime(BWAPI::UnitTypes::Terran_Barracks) / barracksCount;
		if(bigTime < tempTime)
		{
			whichToBuild = BWAPI::UnitTypes::Terran_Barracks;
			bigTime = tempTime;
		}
	}
	if(factoryCount != 0)
	{
		if(temp)
			tempTime = tempFactory->getProductionTime(BWAPI::UnitTypes::Terran_Factory) / factoryCount;
		else
			tempTime = factory->getProductionTime(BWAPI::UnitTypes::Terran_Factory) / factoryCount;
		if(bigTime < tempTime)
		{
			whichToBuild = BWAPI::UnitTypes::Terran_Factory;
			bigTime = tempTime;
		}
	}
	if(machineShopCount != 0)
	{
		if(temp)
			tempTime = tempFactory->getProductionTime(BWAPI::UnitTypes::Terran_Machine_Shop) / machineShopCount;
		else
			tempTime = factory->getProductionTime(BWAPI::UnitTypes::Terran_Machine_Shop) / machineShopCount;
		if(bigTime < tempTime)
		{
			whichToBuild = BWAPI::UnitTypes::Terran_Machine_Shop;
			bigTime = tempTime;
		}
	}
	if(starportCount != 0)
	{
		if(temp)
			tempTime = tempStarport->getProductionTime(BWAPI::UnitTypes::Terran_Starport) / starportCount;
		else
			tempTime = starport->getProductionTime(BWAPI::UnitTypes::Terran_Starport) / starportCount;
		if(bigTime < tempTime)
		{
			whichToBuild = BWAPI::UnitTypes::Terran_Starport;
			bigTime = tempTime;
		}
	}
	if(controlTowerCount != 0)
	{
		if(temp)
			tempTime = tempStarport->getProductionTime(BWAPI::UnitTypes::Terran_Control_Tower) / controlTowerCount;
		else
			tempTime = starport->getProductionTime(BWAPI::UnitTypes::Terran_Control_Tower) / controlTowerCount;
		if(bigTime < tempTime)
		{
			whichToBuild = BWAPI::UnitTypes::Terran_Control_Tower;
			bigTime = tempTime;
		}
	}
	if (bigTime < 1)
	{
		bigTime = 1; // Avoid division by zero errors. It makes no sense for time to be negative either
	}
	return bigTime;
}

int ProductionManager::getTotalMineralCost(bool temp)
{
	int cost = 0;
	if(temp)
	{
		cost += tempBarracks->getMineralCost();
		cost += tempFactory->getMineralCost();
		cost += tempStarport->getMineralCost();
		for(std::map<UnitType, int>::const_iterator i = maintenanceMap.begin(); i != maintenanceMap.end(); i++)
		{
			cost += (i->first.mineralPrice() / i->first.buildTime()) * i->second;
		}
	}
	else
	{
		cost += barracks->getMineralCost();
		cost += factory->getMineralCost();
		cost += starport->getMineralCost();
		for(std::map<UnitType, int>::const_iterator i = maintenanceMap.begin(); i != maintenanceMap.end(); i++)
		{
			cost += (i->first.mineralPrice() / i->first.buildTime()) * i->second;
		}
	}
	return cost;
}

int ProductionManager::getTotalGasCost(bool temp)
{
	int cost = 0;
	if(temp)
	{
		cost += tempBarracks->getGasCost();
		cost += tempFactory->getGasCost();
		cost += tempStarport->getGasCost();
		for(std::map<UnitType, int>::const_iterator i = maintenanceMap.begin(); i != maintenanceMap.end(); i++)
		{
			cost += (i->first.gasPrice() / i->first.buildTime()) * i->second;
		}
	}
	else
	{
		cost += barracks->getGasCost();
		cost += factory->getGasCost();
		cost += starport->getGasCost();
		for(std::map<UnitType, int>::const_iterator i = maintenanceMap.begin(); i != maintenanceMap.end(); i++)
		{
			cost += (i->first.gasPrice() / i->first.buildTime()) * i->second;
		}
	}
	return cost;
}

int ProductionManager::getNumSupplyDepots()
{
	int count = 0;
	for(unsigned int c = 0; c < buildings.size(); c++)
		if(buildings[c]->getType() == UnitTypes::Terran_Supply_Depot)
			count++;
	return count;
}









//int ProductionManager::getFrameETA()
//{
//
//}

void ProductionManager::onFrame()
{
	frameCount++;
	
	if(frameCount % 2 == 0)
	{
		if(!taskMan->expandTaskExists())
			taskMan->setWorkerMax(workMan->getWorkerMax());
		else
			taskMan->setWorkerMax(workMan->getWorkerMax() + 10);
		taskMan->manageTasks();
		workMan->manage();

		bool manageNow = false;
		if(buildCurrentOrder)
			doProduction();

		resolveDependencies(true);
		resolveDependencies(false);
		buildDependencies();
	
		/* test for manageNow states */

		if(frameCount >= 50 || manageNow)
		{
			frameCount = 0;
			setIncomes();
			manage();
		}
	}
}

void ProductionManager::onUnitCreate(Unit* unit)
{
	if(unit->getPlayer() == Broodwar->self())
	{
		if(unit->getType().isBuilding())
			buildings.push_back(unit);
	}
}

void ProductionManager::onUnitDestroy(Unit* unit)
{
	taskMan->checkDestroyed(unit);
	workMan->removeWorker(unit);
	for(unsigned int c = 0; c < buildings.size(); c++)
	{
		if(unit == buildings[c])
		{
			if(unit->getType() == UnitTypes::Terran_Barracks)
				barracksCount -= 1;
			if(unit->getType() == UnitTypes::Terran_Factory)
				factoryCount -= 1;
			if(unit->getType() ==  UnitTypes::Terran_Machine_Shop)
				machineShopCount -= 1;
			if(unit->getType() == UnitTypes::Terran_Starport)
				starportCount -= 1;
			if(unit->getType() == UnitTypes::Terran_Control_Tower)
				controlTowerCount -= 1;

			buildings.erase(buildings.begin() + c);
		}
	}
}

void ProductionManager::onUnitComplete(Unit* unit)
{
	taskMan->addUnit(unit);
	workMan->addWorker(unit);
}

void ProductionManager::prepareTechFor(UnitType unit)
{
	clearDependencies();
	resolveUnitDependency(unit);
	buildDependencies();
}

void ProductionManager::prepareTechFor(UpgradeType ups)
{
	clearDependencies();
	resolveUnitDependency(ups.whatUpgrades());
	requiredUnits.insert(ups.whatUpgrades());
	resolveUnitDependency(ups.whatsRequired(Broodwar->self()->getUpgradeLevel(ups) + 1));
	requiredUnits.insert(ups.whatsRequired(Broodwar->self()->getUpgradeLevel(ups) + 1));
	buildDependencies();
}

void ProductionManager::prepareTechFor(TechType tech)
{
	clearDependencies();
	resolveUnitDependency(tech.whatResearches());
	requiredUnits.insert(tech.whatResearches());
	buildDependencies();
}

void ProductionManager::getAvailableUpgradesFor(UnitType unit)
{
	set<UpgradeType> ups = unit.upgrades();
	set<UpgradeType>::iterator c;
	for(c = ups.begin(); c != ups.end(); c++)
	{
		researchThis(*c);
	}
}

void ProductionManager::maintainThisUnit(UnitType unit, int amount)
{
	prepareTechFor(unit);
	maintenanceMap[unit] = amount;
}

void ProductionManager::removeFromMaintenanceMap(UnitType unit)
{
	maintenanceMap[unit] = 0;
}

void ProductionManager::expandHere(TilePosition tPos, bool ccNeeded)
{
	if(ccNeeded)
		taskMan->addExpansionTask(tPos, expansionPriority);
	else
		taskMan->expand(tPos.x()*32,tPos.y()*32);
}

void ProductionManager::runOrderNow()
{
	switchingOrdersWhenReady = false;
	barracks = tempBarracks;
	factory = tempFactory;
	starport = tempStarport;
	tempBarracks = new ProductionOrder();
	tempFactory = new ProductionOrder();
	tempStarport = new ProductionOrder();
	buildCurrentOrder = true;
	clearDependencies();
	resolveDependencies(false);
	buildDependencies();
}

void ProductionManager::runOrderWhenReady()
{
	switchingOrdersWhenReady = true;
	clearDependencies();
	resolveDependencies(true);
	set<UnitType>::iterator c;
	switchUnits.clear();
	for(c = requiredUnits.begin(); c != requiredUnits.end(); c++)
		switchUnits.insert(*c);
	buildDependencies();
}

void ProductionManager::setAcceptableLosses(float minLosses, float gasLosses)
{
	acceptableMineralLosses = minLosses;
	acceptableGasLosses = gasLosses;
}

void ProductionManager::resetAcceptableLosses()
{
	acceptableMineralLosses = 0;
	acceptableGasLosses = 0;
}

bool ProductionManager::researchThis(UpgradeType ups)
{
	if(actuallyExists(ups.whatUpgrades()) && actuallyExists(ups.whatsRequired(Broodwar->self()->getUpgradeLevel(ups) + 1)))
	{
		taskMan->addUpgradeTask(ups);
		return true;
	}
	taskMan->addUpgradeTask(ups);
	return false;
}

bool ProductionManager::researchThis(TechType tech)
{
	if(actuallyExists(tech.whatResearches()))
	{
		taskMan->addTechTask(tech);
		return true;
	}
	return false;
}

bool ProductionManager::produceThis(UnitType unit)
{
	if(actuallyExists(unit.whatBuilds().first))
	{
		taskMan->addUnitTask(unit,1);
		return true;
	}
	return false;
}

map<UnitType, int> ProductionManager::getMaintenanceMap()
{
	return maintenanceMap;
}

ProductionDetails ProductionManager::loadNewOrder(ProductionOrder *newOrder)
{
	map<UnitType, int> *units = newOrder->getUnits();
	map<UnitType, int>::iterator c;
	tempBarracks->clearOrder();
	tempFactory->clearOrder();
	tempStarport->clearOrder();

	for(c = units->begin(); c != units->end(); c++)
	{
		if((*c).first.whatBuilds().first == UnitTypes::Terran_Barracks)
			tempBarracks->addToOrder((*c).first, (*c).second);
		if((*c).first.whatBuilds().first == UnitTypes::Terran_Factory)
			tempFactory->addToOrder((*c).first, (*c).second);
		if((*c).first.whatBuilds().first == UnitTypes::Terran_Starport)
			tempStarport->addToOrder((*c).first, (*c).second);
	}

	int time1, time2, desiredTime;
	int minCost = getTotalMineralCost(true);
	int gasCost = getTotalGasCost(true);

	if(mineralIncomePerFrame > EPSILON_INCOME) // Cannot compare directly with 0, using estimate for smallest non-zero income
		time1 = int(minCost / mineralIncomePerFrame);
	else time1 = 9999999;
	if(gasIncomePerFrame > EPSILON_INCOME) // Rough estimate of the smallest non-zero gas income possible 
		time2 = int(gasCost / gasIncomePerFrame);
	else time2 = 9999999;

	if(time1 > time2)
		desiredTime = time1;
	else
		desiredTime = time2;

	//Broodwar->printf("%d",desiredTime);

	int requiredRax = tempBarracks->getProductionTime(BWAPI::UnitTypes::Terran_Barracks) / desiredTime; 
	if(requiredRax == 0 && tempBarracks->getProductionTime(BWAPI::UnitTypes::Terran_Barracks) != 0) 
		requiredRax = 1;
	requiredRax -= barracksCount;
	int requiredFac = tempFactory->getProductionTime(BWAPI::UnitTypes::Terran_Factory) / desiredTime;
	if(requiredFac == 0 && tempFactory->getProductionTime(BWAPI::UnitTypes::Terran_Factory) != 0)
		requiredFac = 1;
	requiredFac -= factoryCount;
	int requiredMac = tempFactory->getProductionTime(BWAPI::UnitTypes::Terran_Machine_Shop) / desiredTime;
	if(requiredMac == 0 && tempFactory->getProductionTime(BWAPI::UnitTypes::Terran_Machine_Shop) != 0)
		requiredMac = 1;
	requiredMac -= machineShopCount;
	int requiredStar = tempStarport->getProductionTime(BWAPI::UnitTypes::Terran_Starport) / desiredTime;
	if(requiredStar == 0 && tempStarport->getProductionTime(BWAPI::UnitTypes::Terran_Starport) != 0)
		requiredStar = 1;
	requiredStar -= starportCount;
	int requiredTower = tempStarport->getProductionTime(BWAPI::UnitTypes::Terran_Control_Tower) / desiredTime;
	if(requiredTower == 0 && tempStarport->getProductionTime(BWAPI::UnitTypes::Terran_Control_Tower) != 0)
		requiredTower = 1;
	requiredTower -= controlTowerCount;

	clearDependencies();
	resolveDependencies(true);
	if(gasIncomePerFrame < EPSILON_INCOME && (gasCost != 0 || requiredFac != 0 || requiredStar != 0 || requiredMac != 0 || requiredTower != 0))
		requiredUnits.insert(UnitTypes::Terran_Refinery);
	getDependencyCosts();


	int amt = 0;
	if(requiredUnits.find(UnitTypes::Terran_Barracks) != requiredUnits.end())
		amt = -1;
	if(requiredRax != 0)
		minsReq += (requiredRax + amt) * UnitTypes::Terran_Barracks.mineralPrice();

	amt = 0; 
	if(requiredUnits.find(UnitTypes::Terran_Factory) != requiredUnits.end())
		amt = -1;
	if(requiredFac != 0)
	{
		gasReq += (requiredFac + amt) * UnitTypes::Terran_Factory.gasPrice();
		minsReq += (requiredFac + amt) * UnitTypes::Terran_Factory.mineralPrice();
	}
	amt = 0; 
	if(requiredUnits.find(UnitTypes::Terran_Machine_Shop) != requiredUnits.end())
		amt = -1;
	if(requiredMac != 0)
	{
		gasReq += (requiredMac + amt) * UnitTypes::Terran_Machine_Shop.gasPrice();
		minsReq += (requiredMac + amt) * UnitTypes::Terran_Machine_Shop.mineralPrice();
	}

	amt = 0; 
	if(requiredUnits.find(UnitTypes::Terran_Starport) != requiredUnits.end())
		amt = -1;
	if(requiredStar != 0)
	{
		gasReq += (requiredStar + amt) * UnitTypes::Terran_Starport.gasPrice();
		minsReq += (requiredStar + amt) * UnitTypes::Terran_Starport.mineralPrice();
	}
	amt = 0; 
	if(requiredUnits.find(UnitTypes::Terran_Control_Tower) != requiredUnits.end())
		amt = -1;
	if(requiredTower != 0)
	{
		gasReq += (requiredTower + amt) * UnitTypes::Terran_Control_Tower.gasPrice();
		minsReq += (requiredTower + amt) * UnitTypes::Terran_Control_Tower.mineralPrice();
	}

	int resourceEta = 0;
	if(Broodwar->self()->minerals() < minsReq && mineralIncomePerFrame > EPSILON_INCOME)
		resourceEta = int((minsReq - Broodwar->self()->minerals()) / mineralIncomePerFrame);
	if(Broodwar->self()->gas() < gasReq && gasIncomePerFrame > EPSILON_INCOME)
	{
		if((gasReq - Broodwar->self()->gas()) / gasIncomePerFrame > resourceEta)
			resourceEta = int((gasReq - Broodwar->self()->gas()) / gasIncomePerFrame);
	}
	eta += resourceEta;

	return ProductionDetails(eta, eta + desiredTime + ((requiredRax + requiredFac + requiredStar) * 96), minsReq, gasReq, requiredRax + requiredFac + requiredStar, mineralIncomePerFrame - minCost, gasIncomePerFrame - gasCost);
}


#undef incomePerMiner
#undef incomePerRefiner
#undef dependencyPriority
#undef expansionPriority
#undef unitPriority
#undef productionPriority
#undef supplyRatio
#undef EPSILON_INCOME