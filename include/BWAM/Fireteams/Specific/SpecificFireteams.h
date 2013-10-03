#pragma once
// TODO: Add other includes
#include <BWAM\Fireteams\Fireteam.h>
#include <BWAPI\WeaponType.h>

class Terran_Marine_Fireteam : public Fireteam {
public:
	Terran_Marine_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Marine) {	}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Siege_Tank_Fireteam : public Fireteam {
public:
	
	Terran_Siege_Tank_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode){}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Medic_Fireteam : public Fireteam {
public:
	
	Terran_Medic_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Medic){}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Wraith_Fireteam : public Fireteam {
public:
	Terran_Wraith_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Wraith) {	}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Ghost_Fireteam : public Fireteam {
public:
	Terran_Ghost_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Ghost) {}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Vulture_Fireteam : public Fireteam {
public:
	Terran_Vulture_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Vulture) {}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Goliath_Fireteam : public Fireteam {
public:
	Terran_Goliath_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Goliath) {}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Science_Vessel_Fireteam : public Fireteam {
public:
	Terran_Science_Vessel_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Science_Vessel) {}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Dropship_Fireteam : public Fireteam {
public:
	Terran_Dropship_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Dropship) {	}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Battlecruiser_Fireteam : public Fireteam {
public:
	Terran_Battlecruiser_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Battlecruiser) {}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Firebat_Fireteam : public Fireteam {
public:
	Terran_Firebat_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Firebat) {}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};

class Terran_Valkyrie_Fireteam : public Fireteam {
public:
	Terran_Valkyrie_Fireteam() : Fireteam(BWAPI::UnitTypes::Terran_Valkyrie) {	}
	using Fireteam::deploy; 
	using Fireteam::assist; 
	using Fireteam::updateMicro;
};
