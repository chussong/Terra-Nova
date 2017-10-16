#include "power.hpp"

void PowerGrid::StartTurn() {
	producers.StartTurn();
	consumers.StartTurn();
	BalancePower();
}

void PowerGrid::EndTurn() {
	producers.EndTurn();
	consumers.EndTurn();
}

// Determine if the given building is a producer or consumer of power and add it
// to the appropriate list. It should not be possible for any single building to
// be both.
void PowerGrid::AddBuilding(std::shared_ptr<Building> newBuilding) {
	if (!newBuilding) {
		std::cerr << "Warning: tried to add a null building to a power grid."
			<< std::endl;
		return;
	}

	if (newBuilding->PowerProduction() > 0) {
		producers.push_back(newBuilding); 
	} else {
		if (newBuilding->PowerConsumption() > 0) {
			consumers.push_back(newBuilding); 
		} else {
			std::cerr << "Warning: tried to add a " << newBuilding->Name() << 
				" to a power grid but it neither produces nor consumes power."
				<< std::endl;
			return;
		}
	}
}

void PowerGrid::AddBuilding(std::weak_ptr<Building> newBuilding) {
	AddBuilding(newBuilding.lock());
}

// Find out how much power is being produced, then go through the list of
// consumers from earliest to latest, powering them up if possible and powering
// them down if not (then moving on to the next one in line).
void PowerGrid::BalancePower() {
	int totalPower = 0;
	for (const auto& producer : producers) {
		totalPower += producer->PowerProduction();
	}

	for (auto& consumer : consumers) {
		int powerDraw = consumer->PowerConsumption();
		if (powerDraw <= totalPower) {
			consumer->PowerOn();
			totalPower -= powerDraw;
		} else {
			consumer->PowerOff();
		}
	}
}
