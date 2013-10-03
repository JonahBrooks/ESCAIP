#pragma once
class ProductionDetails
{
private:
	int eta;
	int longEta;
	int cost_Mins;
	int cost_Gas;
	int building;
	float mineralSurplus;
	float gasSurplus;
public:
	int ETA();
	int longETA();
	int costMins();
	int costGas();
	int buildings();
	float surplusMins();
	float surplusGas();

	ProductionDetails(int newEta, int newLongEta, int mins, int gas, int newBuildings, float newMins, float newGas);
	~ProductionDetails(void);
};

