#ifndef PATH_HPP
#define PATH_HPP

#include <vector>
#include <array>
#include <algorithm>
#include <memory>

#include "templates.hpp"
#include "gfxobject.hpp"

struct moveCostTable;
class path {
	std::vector<std::array<unsigned int, 2>> steps;	// each entry is the actual
											// coords of a tile, not increments
	std::vector<gfxObject*> sprites;

	void SpritifyPath();

	public:
		path() = delete;
		path(std::vector<std::array<unsigned int, 2>> steps): steps(steps) {}

		std::array<unsigned int, 2> NextStep() const;
		bool Advance(); // return true if you've arrived at your destination

		gfxObject* FetchPathSegment(const std::array<unsigned int, 2>& start,
				const std::array<unsigned int, 2>& end, const bool endOfPath);
		void RenderStartingFrom(const int spriteX, const int spriteY); 
		SDL_Rect StartingSpriteLayout(const int spriteX, const int spriteY);
		void PathSpriteFromTo(SDL_Rect& layout, const int startX,
				const int startY, const int startRow, const int startColm,
				const int endRow, const int endColm, const bool firstSegment);
};

#endif