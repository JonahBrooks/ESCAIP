#include "ProductionDetails.h"


ProductionDetails::ProductionDetails(int newEta, int newLongEta, int mins, int gas, int newBuildings, float newMins, float newGas)
{
	eta = newEta;
	longEta = newLongEta;
	cost_Mins = mins;
	cost_Gas = gas;
	building = newBuildings;
	mineralSurplus = newMins;
	gasSurplus = newGas;
}


ProductionDetails::~ProductionDetails(void)
{
}

int ProductionDetails::ETA()
{
	return eta;
}

int ProductionDetails::longETA()
{
	return longEta;
}

int ProductionDetails::buildings()
{
	return building;
}

int ProductionDetails::costMins()
{
	return cost_Mins;
}

int ProductionDetails::costGas()
{
	return cost_Gas;
}

float ProductionDetails::surplusMins()
{
	return mineralSurplus;
}

float ProductionDetails::surplusGas()
{
	return gasSurplus;
}