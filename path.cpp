#include "path.hpp"

void path::SpritifyPath(){
	/*std::cout << "Path:";
	for(auto& step : steps) std::cout << "(" << step[0] << "," << step[1] << "), ";
	std::cout << "\b\b " << std::endl;*/
	sprites.resize(steps.size()-1);
	for(auto i = 0u; i < steps.size()-2; ++i){
		sprites[i] = FetchPathSegment(steps[i], steps[i+1], false);
	}
	sprites[sprites.size()-1] = FetchPathSegment(steps[steps.size()-2],
			steps[steps.size()-1], true);
}

// this should properly use a spritesheet system instead of all these names
gfxObject* path::FetchPathSegment(const std::array<unsigned int, 2>& start,
		const std::array<unsigned int, 2>& end, const bool endOfPath){
	std::string name = "path_";
	if(endOfPath){
		name += "arrow_";
	} else {
		name += "segment_";
	}
	if(end[1] > start[1]){
		if(end[0] < start[0]) name += "upright";
		if(end[0] == start[0]) name += "right";
		if(end[0] > start[0]) name += "downright";
	} else {
		if(end[0] < start[0]) name += "upleft";
		if(end[0] == start[0]) name += "left";
		if(end[0] > start[0]) name += "downleft";
	}
	return gfxManager::RequestSprite(name).get();
}

void path::RenderStartingFrom(const int spriteX, const int spriteY){
	if(steps.size() == 0) return;
	if(sprites.size() != steps.size()-1) SpritifyPath();
	/*SDL_Rect spriteLayout(StartingSpriteLayout(spriteX, spriteY));
	sprites[0]->RenderTo(spriteLayout);
	spriteLayout.x = spriteX;
	spriteLayout.y = spriteY;
	spriteLayout.w = TILE_WIDTH;
	spriteLayout.h = TILE_HEIGHT;
	for(auto i = 1u; i < sprites.size(); ++i){
		spriteLayout.y += (steps[i][0] - steps[i-1][0])*TILE_HEIGHT;
		spriteLayout.x += (steps[i][1] - steps[i-1][1])*TILE_WIDTH/2;
		sprites[i]->RenderTo(spriteLayout);
	}*/
	SDL_Rect layout;
	layout.w = TILE_WIDTH;
	layout.h = TILE_HEIGHT;
	int startX, startY;
	for(auto i = 0u; i < sprites.size(); ++i){
		startX = spriteX + ((int)steps[i][1]-(int)steps[0][1])*TILE_WIDTH/2;
		startY = spriteY + ((int)steps[i][0]-(int)steps[0][0])*TILE_HEIGHT;
		PathSpriteFromTo(layout, startX, startY, steps[i][0], steps[i][1],
				steps[i+1][0], steps[i+1][1], false);
		sprites[i]->RenderTo(layout);
	}
}

// startX and startY are the SDL coordinates of the center of the starting tile
void path::PathSpriteFromTo(SDL_Rect& layout, const int startX, 
		const int startY, const int startRow, const int startColm, 
		const int endRow, const int endColm, const bool firstSegment){
	int endX = startX + (endColm - startColm)*TILE_WIDTH/2;
	int endY = startY + (endRow - startRow)*TILE_HEIGHT;
	layout.x = std::min(startX, endX) - 1*TILE_WIDTH/4;
	layout.y = std::max(startY, endY) - TILE_HEIGHT/2;
	if(startRow == endRow){
		layout.x += TILE_WIDTH/4;
		layout.y += TILE_HEIGHT/2;
	}
	/*layout.w = TILE_WIDTH - PERSON_WIDTH/2;
	layout.h = TILE_WIDTH;
	if(steps[1][1] > steps[0][1]){
		layout.x += PERSON_WIDTH/2;
	} else {
		layout.x -= TILE_WIDTH;
	}
	if(steps[1][0] <  steps[0][0]){
		layout.x -= layout.x > spriteX ? TILE_WIDTH/4 : -TILE_WIDTH/4;
		layout.y -= PERSON_HEIGHT;
		layout.h -= PERSON_HEIGHT/2;
	} else if(steps[1][0] > steps[0][0]){
		layout.x -= layout.x > spriteX ? TILE_WIDTH/4 : -TILE_WIDTH/4;
		layout.y += TILE_HEIGHT/2 + PERSON_HEIGHT/2;
		layout.h -= PERSON_HEIGHT/2;
	}*/
}

std::array<unsigned int, 2> path::NextStep() const{
	if(steps.size() == 0){
		std::cerr << "Error: attempted to fetch the next step of a path which "
			<< "does not contain any steps." << std::endl;
		return std::array<unsigned int, 2>({{-1u,-1u}});
	}
	if(steps.size() == 1){
		std::cerr << "Error: attempted to fetch the next step of a path which "
			<< "should have been deleted due to containing exactly one step."
			<< std::endl;
		return std::array<unsigned int, 2>({{-1u,-1u}});
	}
	return steps[1];
}

bool path::Advance(){
	steps.erase(steps.begin());
	sprites.erase(sprites.begin());
	return steps.size() == 1;
}
