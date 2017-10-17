#ifndef TILE_HPP
#define TILE_HPP

#include <vector>
#include <string>
#include <exception>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "gamevars.hpp"
#include "gfxobject.hpp"
#include "building.hpp"
#include "unit.hpp"
#include "colony.hpp"

namespace TerraNova {

class TileType {
	const std::string name;
	const std::array<int, LAST_RESOURCE> yield;

	// this should be a regular pointer to a vector of Building types
	std::vector<std::weak_ptr<BuildingType>> allowedBuildings;
	TileAttributes attributes;

	public:
		TileType() = delete;
		TileType(const std::string name, const std::array<int, LAST_RESOURCE>& yield):
			name(name), yield(yield) {}

		std::string Name() const 							{ return name; }
		std::string Path() const				{return "terrain/" + Name();}
		std::array<int, LAST_RESOURCE> Yield() const 		{ return yield; }
		
		bool AllowedToBuild(const std::string name) const	{ return !name.empty(); }
		void AddAllowedBuilding(const std::shared_ptr<BuildingType> newBldg)
			{ allowedBuildings.push_back(newBldg); }

		bool Cold() const		{ return attributes.cold; }
		bool Wooded() const		{ return attributes.wooded; }
		bool Aquatic() const	{ return attributes.aquatic; }
		bool Hilly() const		{ return attributes.hilly; }
		void SetCold(const bool val)	{ attributes.cold = val; }
		void SetWooded(const bool val)	{ attributes.wooded = val; }
		void SetAquatic(const bool val)	{ attributes.aquatic = val; }
		void SetHilly(const bool val)	{ attributes.hilly = val; }

};

class Tile : public GFXObject {
	TileType* type;
	std::shared_ptr<Building> bldg; // should be unique_ptr
	std::vector<std::weak_ptr<Unit>> weakOccupants;
	std::vector<Unit*> occupants;

	int row;
	int colm;

	bool hasColony = false;
	Colony* linkedColony = nullptr;
	//std::function<void(Colony*)> EnterColony;

	public:
		Tile() = delete;
		Tile(TileType* type, SDL_Renderer* ren,
				const int row, const int colm);
		Tile(const GFXObject& other) = delete;
		Tile(Tile&& other) noexcept : 
			GFXObject(std::move(other)), type(other.type), bldg(other.bldg),
			occupants(other.occupants), row(other.row), colm(other.colm),
			hasColony(other.hasColony) {}
		Tile& operator=(const GFXObject& other) = delete;

		bool IsTile() const { return true; }

		void StartTurn();
		void EndTurn();

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);
		void Resize(int w, int h);
		void Resize(SDL_Rect newLayout);

		bool InsideQ(const int x, const int y);
		GFXObject* SelectAt(const int x, const int y);
		//int Select();
		bool Click();
		TileType* Type() const;
		std::string Name() const { return Type()->Name(); }
		void SetTileType(TileType* newType);
		std::array<int, LAST_RESOURCE> Income() const;
		bool Cold() const		{ return Type()->Cold(); }
		bool Wooded() const		{ return Type()->Wooded(); }
		bool Aquatic() const	{ return Type()->Aquatic(); }
		bool Hilly() const		{ return Type()->Hilly(); }

		bool HasColony() const;
		Colony* LinkedColony() const;
		void SetColony(Colony* newColony);
		void LinkColony(Colony* colonyToLink);

		void SetLocation(const int row, const int colm);
		int Row() const;
		int Colm() const;

		void AddBuilding(std::shared_ptr<Building> newBldg);
		void AddBuilding(const BuildingType* type);
		void RemoveBuilding();

		bool AddOccupant(std::shared_ptr<Unit> newOccupant);
		bool RemoveOccupant(Unit* removeThis);
		std::vector<Unit*> Occupants() const;
		std::shared_ptr<Unit> SharedOccupant(const unsigned int i) const;
		unsigned int NumberOfOccupants() const { return weakOccupants.size(); }
		unsigned int NumberOfLivingOccupants() const;
		Unit* Defender() const;
		int Faction() const;

		void Training();

		static unsigned int MoveCost(const Tile& destination,
			const MoveCostTable moveCosts);
};

} // namespace TerraNova

#endif
