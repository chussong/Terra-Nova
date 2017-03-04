#include "tile.hpp"

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
