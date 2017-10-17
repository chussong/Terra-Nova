#ifndef BUILDING_HPP
#define BUILDING_HPP

#include <memory>
#include "gamevars.hpp"
#include "gfxobject.hpp"
#include "unit.hpp"

namespace TerraNova {

constexpr std::array<int, LAST_RESOURCE> defaultCost{};

class BuildingType{
	const int id;
	const std::string name;
	const std::array<int, LAST_RESOURCE> cost;
	const int buildTime;

	//std::vector<std::weak_ptr<TileType>> allowedTerrain; // empty means all terrain allowed
	// instead of allowedTerrain, use these
	TileAttributes allowedTerrain;

	bool canHarvest = true;
	bool automatic = false;	// i.e. automatically harvests resources on its Tile
	unsigned int maxOccupants = 1;
	int powerConsumption = 0; // set to negative value for power producers
	std::array<int, LAST_RESOURCE> bonusResources = {{0}};
	std::vector<std::shared_ptr<UnitType>> canTrain;
	
	public:
		BuildingType() = delete;
		BuildingType(const int id,
				const std::string name, 
				const std::array<int, LAST_RESOURCE> cost,
				const int buildTime)
					:id(id), name(name), cost(cost), buildTime(buildTime) {}

		int								ID()		const;
		std::string						Name()		const;
		std::array<int, LAST_RESOURCE>	Cost()		const;
		bool CanBuyWith(std::array<int,LAST_RESOURCE> availableResources) const;
		int								BuildTime() const;

		//void SetAllowedTerrain(const std::vector<std::shared_ptr<TileType>>& val);
		//std::vector<std::shared_ptr<TileType>> AllowedTerrain() const;

		bool AquaticAllowed() const { return allowedTerrain.aquatic; }
		bool ColdAllowed()    const { return allowedTerrain.cold; }
		bool HillyAllowed()   const { return allowedTerrain.hilly; }
		bool WoodedAllowed()  const { return allowedTerrain.wooded; }

		void SetCanHarvest(const bool val);
		bool CanHarvest() const;
		void SetAutomatic(const bool val);
		bool Automatic() const;
		void SetMaxOccupants(const int val);
		unsigned int MaxOccupants() const;
		void SetPowerProduction(const int val);
		int PowerProduction() const;
		void SetPowerConsumption(const int val);
		int PowerConsumption() const;
		void SetBonusResources(const std::array<int, LAST_RESOURCE>& val);
		std::array<int, LAST_RESOURCE> BonusResources() const;
		void SetCanTrain(const std::vector<std::shared_ptr<UnitType>>& val);
		void SetCanTrain(const std::shared_ptr<UnitType> val);
		std::vector<std::shared_ptr<UnitType>> CanTrain() const;

};

class BuildingPrototype : public GFXObject {
	const BuildingType* type;

	public:
		BuildingPrototype() = delete;
		BuildingPrototype(SDL_Renderer* ren, const int x, const int y, 
				const BuildingType* type) : 
			GFXObject(ren, "buildings/" + type->Name() + "/sprite", x, y), 
			type(type) {}
		BuildingPrototype(SDL_Renderer* ren, const int x, const int y,
				const std::shared_ptr<BuildingType> type) : 
			GFXObject(ren, "buildings/" + type->Name() + "/sprite", x, y), 
			type(type.get()) {}
		BuildingPrototype(const BuildingPrototype& other) = delete;
		BuildingPrototype(BuildingPrototype&& other) noexcept : 
			GFXObject(std::move(other)), type(std::move(other.type)) {}
		BuildingPrototype& operator=(const BuildingPrototype& other) = delete;

		bool IsBuildingPrototype() const { return true; }

		std::string Name() const;
		std::array<int, LAST_RESOURCE>	Cost() const;
		bool CanBuyWith(std::array<int,LAST_RESOURCE> availableResources) const;
		int PowerProduction() const;
		int PowerConsumption() const;
		int BuildTime() const;
		const BuildingType* Type() const;
};

class Building : public GFXObject {
	const BuildingType* type;
	int turnsLeft;
	bool poweredOn = false;
	char faction;
	//int health = 100;
	//int upgradeLevel = 1;
	//std::vector<std::shared_ptr<Unit>> occupants;
	
	std::shared_ptr<UnitType> nowTraining;
	int turnsToTrain;

	public:
		Building() = delete;
		Building(SDL_Renderer* ren, const int x, const int y, 
				const BuildingType* type) : 
			GFXObject(ren, "buildings/" + type->Name() + "/sprite", x, y), 
			type(type), turnsLeft(type->BuildTime()) {}
		Building(SDL_Renderer* ren, const int x, const int y,
				const std::shared_ptr<BuildingType> type) : 
			GFXObject(ren, "buildings/" + type->Name() + "/sprite", x, y), 
			type(type.get()), turnsLeft(type->BuildTime()) {}
		Building(const Building& other) = delete;
		Building(Building&& other) noexcept : 
			GFXObject(std::move(other)), type(std::move(other.type)),
			turnsLeft(std::move(other.turnsLeft)) {}
		Building& operator=(const Building& other) = delete;

		bool IsBuilding() const { return true; }

		std::string Name() const;
		std::array<int, LAST_RESOURCE>	Cost() const;
		bool CanBuyWith(std::array<int,LAST_RESOURCE> availableResources) const;
		int BuildTime() const;
		char Faction() const { return faction; }
		void SetFaction(const char newFaction) { faction = newFaction; }

		void StartConstruction();
		int TurnsLeft() const;
		void BuildTurn();
		bool Finished() const;

		bool CanHarvest() const;
		int PowerProduction() const;
		int PowerConsumption() const;
		bool PoweredOn() const;
		bool PowerOn();
		void PowerOff();

		bool Automatic() const;
		unsigned int MaxOccupants() const;
		std::array<int, LAST_RESOURCE> BonusResources() const;

		std::vector<std::shared_ptr<UnitType>> CanTrain() const;
		void StartTraining(std::shared_ptr<UnitType> newSpec);
		int TurnsToTrain() const;
		std::shared_ptr<UnitType> NowTraining() const;
		void TrainingTurn();
		void FinishTraining();
};

} // namespace TerraNova
#endif
