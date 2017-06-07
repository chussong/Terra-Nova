#ifndef TILE_HPP
#define TILE_HPP

#include <vector>
#include <string>
#include <exception>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "entity.hpp"
#include "building.hpp"
#include "person.hpp"

class person;
class building;
class buildingType;

class tileType {
	const std::string name;
	const std::array<int, LAST_RESOURCE> yield;
	const int moveCost;

	std::vector<std::shared_ptr<buildingType>> allowedBuildings;
	bool cold = false;
	bool wooded = false;
	bool aquatic = false;

	public:
		tileType() = delete;
		tileType(const std::string name, const std::array<int, LAST_RESOURCE>& yield,
				const int moveCost): name(name), yield(yield), moveCost(moveCost) {}

		std::string Name() const 							{ return name; }
		std::string Path() const				{return "terrain/" + Name();}
		std::array<int, LAST_RESOURCE> Yield() const 		{ return yield; }
		int MoveCost() const								{ return moveCost; }
		
		bool AllowedToBuild(const std::string name) const	{ return true; }
		void AddAllowedBuilding(const std::shared_ptr<buildingType> newBldg)
			{ allowedBuildings.push_back(newBldg); }

		bool Cold() const		{ return cold; }
		bool Wooded() const		{ return wooded; }
		bool Aquatic() const	{ return aquatic; }
		void SetCold(const bool val)	{ cold = val; }
		void SetWooded(const bool val)	{ wooded = val; }
		void SetAquatic(const bool val)	{ aquatic = val; }

};

class tile : public entity {
	std::shared_ptr<tileType> type;
	std::shared_ptr<building> bldg;
	std::vector<std::shared_ptr<person>> occupants;

	int row;
	int colm;

	bool hasColony = false;

	public:
		tile() = delete;
		tile(std::shared_ptr<tileType> type, SDL_Renderer* ren,
				const int row, const int colm);
		tile(const entity& other) = delete;
		tile(tile&& other) noexcept : 
			entity(std::move(other)), type(other.type), bldg(other.bldg),
			occupants(other.occupants), row(other.row), colm(other.colm),
			hasColony(other.hasColony) {}
		tile& operator=(const entity& other) = delete;

		void Render() const;
		void MoveTo(int x, int y);
		void MoveTo(SDL_Rect newLayout);
		void Resize(int w, int h);
		void Resize(SDL_Rect newLayout);

		bool InsideQ(const int x, const int y) const;
		int Select();
		std::shared_ptr<tileType> TileType() const;
		std::string Name() const { return TileType()->Name(); }
		void SetTileType(const std::shared_ptr<tileType> newType);
		std::array<int, LAST_RESOURCE> Income() const;

		bool HasColony() const;
		void SetHasColony(const bool val);

		void SetLocation(const int row, const int colm);
		int Row() const;
		int Colm() const;

		void AddBuilding(std::shared_ptr<building> newBldg);
		void RemoveBuilding();

		bool AddOccupant(std::shared_ptr<person> newOccupant);
		bool RemoveOccupant(std::shared_ptr<person> removeThis);
		std::vector<std::shared_ptr<person>> Occupants() const;
		std::shared_ptr<person> Defender() const;
		char Owner() const;

		void Training();
};

#endif
