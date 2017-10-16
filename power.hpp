#ifndef POWER_HPP
#define POWER_HPP

#include <iostream>
#include <memory>

#include "templates.hpp"
#include "building.hpp"

class PowerGrid {
	public:
		void StartTurn(); // check power supply and consumption here
		void EndTurn();

		void AddBuilding(std::shared_ptr<Building> newBuilding);
		void AddBuilding(std::weak_ptr<Building> newBuilding);

	private:
		WeakVector<Building> producers;
		WeakVector<Building> consumers;

		void BalancePower();
};

#endif
