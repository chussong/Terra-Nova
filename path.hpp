#ifndef PATH_HPP
#define PATH_HPP

#include <vector>
#include <array>
#include <algorithm>
#include <memory>

#include "templates.hpp"
#include "file.hpp"
#include "sprite.hpp"

namespace TerraNova {

class Path {
	std::vector<std::array<unsigned int, 2>> steps;	// each entry is the actual
											// coords of a Tile, not increments
	std::vector<Sprite*> sprites;

	void SpritifyPath();

	public:
		Path() = delete;
		Path(std::vector<std::array<unsigned int, 2>> steps): steps(steps) {}

		std::array<unsigned int, 2> NextStep() const;
		bool Advance(); // return true if you've arrived at your destination

		Sprite* FetchPathSegment(const std::array<unsigned int, 2>& start,
				const std::array<unsigned int, 2>& end, const bool endOfPath);
		void RenderStartingFrom(const int spriteX, const int spriteY); 
		SDL_Rect StartingSpriteLayout(const int spriteX, const int spriteY);
		void PathSpriteFromTo(SDL_Rect& layout, const int startX,
				const int startY, const int startRow, const int startColm,
				const int endRow, const int endColm, const bool firstSegment);
};

} // namespace TerraNova

#endif
