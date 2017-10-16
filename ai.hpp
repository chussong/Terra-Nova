#ifndef AI_HPP
#define AI_HPP

#include <vector>
#include <iostream>

#include "unit.hpp"
#include "map.hpp"
#include "tile.hpp"
#include "colony.hpp"
#include "random.hpp"

namespace TerraNova {

enum AIPlan_t { PLAN_ATTACK = 0, PLAN_DEFEND = 1, PLAN_ECONOMY = 2,
	PLAN_BUILDARMY = 3};

struct AIPriority {
	int priority = 0;
	AIPlan_t action;
	Tile* target = nullptr;

	AIPriority(AIPlan_t action, Tile* target = nullptr): action(action),
			target(target) {}
};

class TacticalAI {
	Map& map;
	int faction;

	WeakVector<Unit> myUnits;
	WeakVector<Unit> enemyUnits;

	Unit* FindNearestEnemy(const Unit& attacker);
	unsigned int TargetPriority(const Unit& target, 
			const unsigned int distance) const;

	public:
		TacticalAI(Map& map, const int faction): map(map), faction(faction) {}

		void AddUnit(std::shared_ptr<Unit> newUnit);

		void StartTurn();
		void EndTurn();

		void GiveOrders();
};

class AIPlayer {
	Map& map;
	int faction;

	std::vector<TacticalAI> tacticalAIs; // one for each dynamic theater

	std::vector<AIPriority> priorities;

	std::vector<std::weak_ptr<Colony>> myWeakColonies;
	std::vector<Colony*> myColonies;
	std::vector<std::weak_ptr<Unit>> myWeakUnits;
	std::vector<Unit*> myUnits;

	int ColonyDefensePriority(const Colony& colony);
	int ColonyAttackPriority(const Colony& colony);
	int UnitAttackPriority(const Unit& unit);
	int DevelopmentPriority(const Colony& colony);

	// i.e. compare friendly and enemy forces nearby, return a positive value 
	// for favorable fight conditions, negative value for unfavorable ones
	int ForceComparison(const Colony& colony);

	// compute strength based on attack & defense, then weight by distance away
	int WeightedUnitStrength(const Unit& unit);

	//int MeleeStrength(const Region& region);
	//std::vector<Unit*> FindMeleeToTransfer(const Region& region);
	Colony* FindBestPlaceToBuild(const UnitType& type);

	public:
		AIPlayer(Map& map, const int faction);

		void AddUnit(std::shared_ptr<Unit> newUnit);

		void StartTurn();
		void EndTurn();

		void GiveOrders();
};

} // namespace TerraNova

#endif
