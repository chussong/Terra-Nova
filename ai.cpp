#include "ai.hpp"

void TacticalAI::AddUnit(std::shared_ptr<Unit> newUnit){
	if(!newUnit){
		std::cerr << "Warning: attempted to add a nullptr Unit to a TacticalAI."
			<< std::endl;
		return;
	}
	if(newUnit->Faction() == faction){
		myUnits.push_back(newUnit);
	} else {
		enemyUnits.push_back(newUnit);
	}
}

void TacticalAI::StartTurn(){
	myUnits.StartTurn();
	enemyUnits.StartTurn();
}

void TacticalAI::EndTurn(){
	myUnits.EndTurn();
	enemyUnits.EndTurn();
}

void TacticalAI::GiveOrders(){
	/*std::cout << "Giving orders to " << myUnits.size() << " units to attack "
		<< enemyUnits.size() << " enemies." << std::endl;
	std::cout << "My units:" << std::endl;
	for(auto& buddy : myUnits) std::cout << buddy->Spec()->Name() << std::endl;
	std::cout << "Enemy units:" << std::endl;
	for(auto& jerk : enemyUnits) std::cout << jerk->Spec()->Name() << std::endl;*/
	for(auto& chump : myUnits){
		if(enemyUnits.size() == 0 || !chump->CanAttack()){
			chump->OrderPatrol();
			continue;
		}
		Unit* target = FindNearestEnemy(*chump);
		if(target){
			chump->OrderMove(map.PathTo(
					chump->Location(), target->Location(), chump->MoveCosts()));
		} else {
			chump->OrderPatrol();
		}
	}
}

Unit* TacticalAI::FindNearestEnemy(const Unit& attacker){
	if(enemyUnits.size() == 0){
		std::cerr << "Warning: TacticalAI asked to find nearest enemy, but no "
			<< "enemies were known." << std::endl;
		return nullptr;
	}
	unsigned int bestPriority = -1u;
	Unit* closestEnemy = nullptr;
	unsigned int currentDistance, currentPriority;
	for(auto i = 0u; i < enemyUnits.size(); ++i){
		currentDistance = map.DistanceBetween(attacker.Location(), 
				enemyUnits[i]->Location(), attacker.MoveCosts());
		currentPriority = TargetPriority(*enemyUnits[i], currentDistance);
		if(currentPriority < bestPriority 
				|| ((currentPriority == bestPriority) && Random::Bool())){
			closestEnemy = enemyUnits[i];
			bestPriority = currentPriority;
		}
	}
	return closestEnemy;
}

unsigned int TacticalAI::TargetPriority(const Unit& target, 
		const unsigned int distance) const{
	return 1000*distance + (100*target.Health())/target.MaxHealth();
}

AIPlayer::AIPlayer(Map& map, const int faction): map(map), faction(faction){
	tacticalAIs.emplace_back(map, faction);
}

// temporary placeholder
void AIPlayer::AddUnit(std::shared_ptr<Unit> newUnit){
	tacticalAIs[0].AddUnit(newUnit);
}

void AIPlayer::StartTurn(){
	for(auto& tai : tacticalAIs) tai.StartTurn();
}

void AIPlayer::EndTurn(){
	for(auto& tai : tacticalAIs) tai.EndTurn();
}

void AIPlayer::GiveOrders(){
	for(auto& tai : tacticalAIs) tai.GiveOrders();
}
