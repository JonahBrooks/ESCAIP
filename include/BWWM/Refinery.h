#include <BWAPI.h>
#include <BWTA.h>
using namespace BWAPI;
using namespace BWTA;
using namespace std;

class Refinery
{
private:
	vector <Unit*> gatherers;
	Unit* refinery;
	int maxCollectors;
	void grabWorker();
public:
	Refinery(Unit* newRefinery);
	~Refinery(void);

	void manageRefinery();
	void setMax(int newMax);
	bool inExistence();
	bool isSat();
};


