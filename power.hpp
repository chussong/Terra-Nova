#ifndef POWER_HPP
#define POWER_HPP

#include <iostream>
#include <memory>
#include <vector>

#include "templates.hpp"
#include "building.hpp"

namespace TerraNova {

class PowerGrid {
	public:
		void StartTurn(); // check power supply and consumption here
		void EndTurn();

		void AddBuilding(std::shared_ptr<Building> newBuilding);
		void AddBuilding(std::weak_ptr<Building> newBuilding);

		const int& AvailablePower() const;
		const int& MaximumPower() const;

	private:
		WeakVector<Building> producers;
		WeakVector<Building> consumers;

		int availablePower = 0;
		int maximumPower = 0;

		void BalancePower();
};

} // namespace TerraNova

#endif
