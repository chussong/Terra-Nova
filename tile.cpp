#include "tile.hpp"

tile::tile(terrain_t tileType, SDL_Renderer* ren, const std::string spriteFile,
		const int x, const int y): entity(ren, spriteFile, x, y), tileType(tileType)
{
/*	std::cout << "Using the renderer at " << ren <<
		", I have rendered the sprite at " << spriteFile <<
		", resulting in a sprite at " << sprite.get() << std::endl;*/
}

bool tile::InsideQ(const int x, const int y) const {
	int relX = x - layout.x;
	int relY = y - layout.y;
	if(relX < 0 || relY < 0 || relX > layout.w || relY > layout.h ||
			(4*relY < layout.h &&
			 ((2*relX < layout.w - 3.464*relY) || 
			 (2*relX > layout.w + 3.464*relY))) ||
			(4*relY > 3*layout.h &&
			 ((4*relX < 1.732*(4*relY - 3*layout.h)) || 
			 (4*relX > 4*layout.w - 1.732*(4*relY - 3*layout.h)))))
		return false;
	return true;
}

terrain_t tile::TileType() const{
	return tileType;
}

std::array<int, LAST_RESOURCE> tile::Income() const{
	std::array<int, LAST_RESOURCE> inc = {{0}};
	switch(tileType){
		case PLAINS:	inc[FOOD] = 4;
						return inc;
		case MOUNTAIN:	inc[IRON] = 4;
		default:		return inc;
	}
}
