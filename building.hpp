#ifndef BUILDING_HPP
#define BUILDING_HPP

#include <memory>
#include "gamevars.hpp"

constexpr std::array<int, LAST_RESOURCE> defaultCost{};

class buildingType{
	const int id;
	const std::string name;
	const std::array<int, LAST_RESOURCE> cost;
	const int buildTime;

	std::vector<terrain_t> allowedTerrain; // empty means all terrain allowed
	bool automatic = false;	// i.e. automatically harvests resources on its tile
	unsigned int maxOccupants = 1;
	std::array<int, LAST_RESOURCE> bonusResources = {{0}};
	
	public:
		buildingType() = delete;
		buildingType(const int id,
				const std::string name, 
				const std::array<int, LAST_RESOURCE> cost,
				const int buildTime)
					:id(id), name(name), cost(cost), buildTime(buildTime) {}

		int								ID()		const	{return id; }
		std::string						Name()		const	{return name; }
		std::array<int, LAST_RESOURCE>	Cost()		const	{return cost; }
		int								BuildTime() const	{return buildTime; }

		void SetAutomatic(const bool val)	{automatic = val;}
		bool Automatic() const				{return automatic;}
		void SetMaxOccupants(const int val)	{maxOccupants = val;}
		unsigned int MaxOccupants() const	{return maxOccupants;}
		void SetBonusResources(const std::array<int, LAST_RESOURCE>& val)
			{bonusResources = val;}
		std::array<int, LAST_RESOURCE> BonusResources() const
			{return bonusResources;}
		void SetAllowedTerrain(const std::vector<terrain_t>& val) 
			{allowedTerrain = val;}
		std::vector<terrain_t> AllowedTerrain() const {return allowedTerrain;}

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

		std::string Name() const						{return type->Name();}
		std::array<int, LAST_RESOURCE>	Cost() const	{return type->Cost();}
		int BuildTime() const						{return type->BuildTime();}
		std::shared_ptr<buildingType> Type() const		{return type;}
};

class building : public entity {
	const std::shared_ptr<buildingType> type;
	int turnsLeft;
	//int health = 100;
	//int upgradeLevel = 1;
	//std::vector<person> occupants;

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

		std::string Name() const						{return type->Name();}
		std::array<int, LAST_RESOURCE>	Cost() const	{return type->Cost();}
		int BuildTime() const						{return type->BuildTime();}

		void StartConstruction()					{turnsLeft = BuildTime();}
		int TurnsLeft() const 						{return turnsLeft;}
		void NextTurn()								{turnsLeft--;}
		bool Finished() const						{return turnsLeft == 0;}

		bool Automatic() const				{return type->Automatic();}
		unsigned int MaxOccupants() const	{return type->MaxOccupants();}
		std::array<int, LAST_RESOURCE> BonusResources() const
			{return type->BonusResources();}
};

#endif
