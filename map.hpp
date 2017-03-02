#ifndef MAP_HPP
#define MAP_HPP

enum terrain_t { OCEAN = 0, COAST = 1, PLAINS = 10, MOUNTAIN = 100 };

#include <vector>
#include <memory>
#include <string>
#include "colony.hpp"
#include "templates.hpp"

class colony;

constexpr int DEFAULT_WIDTH = 100;
constexpr int DEFAULT_HEIGHT = 100;

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

		static std::string TerrainName(const terrain_t type);
};
#endif
