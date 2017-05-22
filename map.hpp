#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <memory>
#include <string>
#include "templates.hpp"
#include "gamevars.hpp"
#include "tile.hpp"

class colony;
class person;

class map {
	std::vector<std::vector<terrain_t>> terrain;
	std::vector<std::weak_ptr<colony>> colonies;
	std::vector<std::weak_ptr<person>> roamers;

	void Clean();

	static void InitTerrain(std::vector<std::vector<terrain_t>>& terrain);

	public:
		map();

		void AddColony(const std::shared_ptr<colony> colony);
		std::shared_ptr<colony> Colony(const int num);
		const std::shared_ptr<colony> Colony(const int num) const;
		terrain_t Terrain(const int row, const int column) const;

		std::string TerrainName(const unsigned int x, const unsigned int y);
		static std::string TerrainName(const terrain_t type);
};
#endif
