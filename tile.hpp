#ifndef TILE_HPP
#define TILE_HPP

#include <vector>
#include <string>
#include <exception>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "gfxobject.hpp"
#include "building.hpp"
#include "unit.hpp"

class TileType {
	const std::string name;
	const std::array<int, LAST_RESOURCE> yield;

	// this should be a regular pointer to a vector of Building types
	std::vector<std::weak_ptr<BuildingType>> allowedBuildings;
	bool cold = false;
	bool wooded = false;
	bool aquatic = false;
	bool hilly = false;

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

		bool Cold() const		{ return cold; }
		bool Wooded() const		{ return wooded; }
		bool Aquatic() const	{ return aquatic; }
		bool Hilly() const		{ return hilly; }
		void SetCold(const bool val)	{ cold = val; }
		void SetWooded(const bool val)	{ wooded = val; }
		void SetAquatic(const bool val)	{ aquatic = val; }
		void SetHilly(const bool val)	{ hilly = val; }

};

class Tile : public GFXObject {
	std::weak_ptr<TileType> type;
	std::shared_ptr<Building> bldg;
	std::vector<std::weak_ptr<Unit>> weakOccupants;
	std::vector<Unit*> occupants;

	int row;
	int colm;

	bool hasColony = false;
	//std::function<void(Colony*)> EnterColony

	public:
		Tile() = delete;
		Tile(std::shared_ptr<TileType> type, SDL_Renderer* ren,
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

		bool InsideQ(const int x, const int y) const;
		// deprecate Select() when possible
		int Select();
		bool Click();
		std::shared_ptr<TileType> Type() const;
		std::string Name() const { return Type()->Name(); }
		void SetTileType(const std::shared_ptr<TileType> newType);
		std::array<int, LAST_RESOURCE> Income() const;
		bool Cold() const		{ return Type()->Cold(); }
		bool Wooded() const		{ return Type()->Wooded(); }
		bool Aquatic() const	{ return Type()->Aquatic(); }
		bool Hilly() const		{ return Type()->Hilly(); }

		bool HasColony() const;
		void SetHasColony(const bool val);

		void SetLocation(const int row, const int colm);
		int Row() const;
		int Colm() const;

		void AddBuilding(std::shared_ptr<Building> newBldg);
		void RemoveBuilding();

		bool AddOccupant(std::shared_ptr<Unit> newOccupant);
		bool RemoveOccupant(Unit* removeThis);
		std::vector<Unit*> Occupants() const;
		unsigned int NumberOfOccupants() const;
		Unit* Defender() const;
		char Owner() const;

		void Training();

		static unsigned int MoveCost(const Tile& destination,
			const MoveCostTable moveCosts);
};

#endif
