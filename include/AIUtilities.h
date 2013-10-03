#pragma once
#include <map>
#include <queue>
#include <vector>
#include <BWAPI.h>
#include <BWTA.h>

class AIUtilities {
    private:
        static BWAPI::AIModule *module;
		static std::set<BWAPI::Unit*> geysers;
		AIUtilities(){}

        class BaseLocationCompare {
            private:
                BWTA::BaseLocation *fromBase;
            public:
                BaseLocationCompare(BWTA::BaseLocation *fromBaseArg) { fromBase = fromBaseArg; }
                bool operator() (const BWTA::BaseLocation *left, const BWTA::BaseLocation *right) const
                {
                    return (left->getGroundDistance(fromBase) > right->getGroundDistance(fromBase));
                }
        };

    public:
        static void AIUtilities::init(BWAPI::AIModule*);

        static std::priority_queue<BWTA::BaseLocation*, std::vector<BWTA::BaseLocation*>, BaseLocationCompare> expansionQueue;
        static std::priority_queue<BWTA::BaseLocation*, std::vector<BWTA::BaseLocation*>, BaseLocationCompare> enemyExpansionQueue;

        static std::vector<BWTA::BaseLocation*> expansionVector;
        static std::vector<BWTA::BaseLocation*> enemyExpansionVector;

        static std::map<std::string, BWTA::BaseLocation*> expansionMap;
        static std::map<std::string, BWTA::BaseLocation*> enemyExpansionMap;

		static void trackGeyser(BWAPI::Unit *geyser);

		static void onFrame();

};

