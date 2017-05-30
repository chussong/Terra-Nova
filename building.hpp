#ifndef BUILDING_HPP
#define BUILDING_HPP

#include <memory>
#include "gamevars.hpp"
#include "person.hpp"

constexpr std::array<int, LAST_RESOURCE> defaultCost{};

class unitSpec;
class buildingType{
	const int id;
	const std::string name;
	const std::array<int, LAST_RESOURCE> cost;
	const int buildTime;

	std::vector<terrain_t> allowedTerrain; // empty means all terrain allowed
	bool canHarvest = true;
	bool automatic = false;	// i.e. automatically harvests resources on its tile
	unsigned int maxOccupants = 1;
	std::array<int, LAST_RESOURCE> bonusResources = {{0}};
	std::vector<std::shared_ptr<unitSpec>> canTrain;
	
	public:
		buildingType() = delete;
		buildingType(const int id,
				const std::string name, 
				const std::array<int, LAST_RESOURCE> cost,
				const int buildTime)
					:id(id), name(name), cost(cost), buildTime(buildTime) {}

		int								ID()		const;
		std::string						Name()		const;
		std::array<int, LAST_RESOURCE>	Cost()		const;
		int								BuildTime() const;

		void SetAllowedTerrain(const std::vector<terrain_t>& val);
		std::vector<terrain_t> AllowedTerrain() const;
		void SetCanHarvest(const bool val);
		bool CanHarvest() const;
		void SetAutomatic(const bool val);
		bool Automatic() const;
		void SetMaxOccupants(const int val);
		unsigned int MaxOccupants() const;
		void SetBonusResources(const std::array<int, LAST_RESOURCE>& val);
		std::array<int, LAST_RESOURCE> BonusResources() const;
		void SetCanTrain(const std::vector<std::shared_ptr<unitSpec>>& val);
		void SetCanTrain(const std::shared_ptr<unitSpec> val);
		std::vector<std::shared_ptr<unitSpec>> CanTrain() const;

};

class buildingPrototype : public entity {
	const std::shared_ptr<buildingType> type;

	public:
		buildingPrototype() = delete;
		buildingPrototype(SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y,
				const std::shared_ptr<buildingType> type) : 
			entity(ren, spriteFile, x, y), type(type) {}
		buildingPrototype(const buildingPrototype& other) = delete;
		buildingPrototype(buildingPrototype&& other) noexcept : 
			entity(std::move(other)), type(std::move(other.type)) {}
		buildingPrototype& operator=(const buildingPrototype& other) = delete;

		std::string Name() const;
		std::array<int, LAST_RESOURCE>	Cost() const;
		int BuildTime() const;
		std::shared_ptr<buildingType> Type() const;
};

class building : public entity {
	const std::shared_ptr<buildingType> type;
	int turnsLeft;
	//int health = 100;
	//int upgradeLevel = 1;
	//std::vector<std::shared_ptr<person>> occupants;
	
	std::shared_ptr<unitSpec> nowTraining;
	int turnsToTrain;

	public:
		building() = delete;
		building(SDL_Renderer* ren, const std::string spriteFile,
				const int x, const int y,
				const std::shared_ptr<buildingType> type) : 
			entity(ren, spriteFile, x, y), type(type) {}
		building(const building& other) = delete;
		building(building&& other) noexcept : 
			entity(std::move(other)), type(std::move(other.type)),
			turnsLeft(std::move(other.turnsLeft)) {}
		building& operator=(const building& other) = delete;

		std::string Name() const;
		std::array<int, LAST_RESOURCE>	Cost() const;
		int BuildTime() const;

		void StartConstruction();
		int TurnsLeft() const;
		void BuildTurn();
		bool Finished() const;
		bool CanHarvest() const;

		bool Automatic() const;
		unsigned int MaxOccupants() const;
		std::array<int, LAST_RESOURCE> BonusResources() const;

		std::vector<std::shared_ptr<unitSpec>> CanTrain() const;
		void StartTraining(std::shared_ptr<unitSpec> newSpec);
		int TurnsToTrain() const;
		std::shared_ptr<unitSpec> NowTraining() const;
		void TrainingTurn();
		void FinishTraining();
};

#endif
