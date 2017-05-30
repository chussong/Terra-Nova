#include "entity.hpp"


entity::entity(SDL_Renderer* ren, const std::string& spriteFile,
		const int x, const int y, const bool selectable): ren(ren),
		selectable(selectable) {
	layout.x = x;
	layout.y = y;
	sprite = std::make_unique<gfxObject>(ren, spriteFile, layout);
	if(selectable){
		std::string selectedFile(spriteFile);
		selectedFile.insert(selectedFile.size()-4, "_selected");
		selectedSprite = std::make_unique<gfxObject>(ren, selectedFile, layout);
	}
}

void entity::Render() const{
	if(selected){
		selectedSprite->RenderTo(ren, layout);
	} else {
		sprite->RenderTo(ren, layout);
	}
}

int entity::Select(){
	if(selectable){
		selected = true;
	}
	return static_cast<int>(SELECTED);
}

void entity::Deselect(){
	selected = false;
}

void entity::MoveTo(int x, int y){
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	layout.x = std::min(x, SCREEN_WIDTH - layout.w);
	layout.y = std::min(y, SCREEN_HEIGHT - layout.h);
}

// this takes an entire SDL_Rect but only uses the positions, not the sizes
void entity::MoveTo(SDL_Rect newLayout){
	if(newLayout.x < 0) newLayout.x = 0;
	if(newLayout.y < 0) newLayout.y = 0;
	layout.x = std::min(newLayout.x, SCREEN_WIDTH - layout.w);
	layout.y = std::min(newLayout.y, SCREEN_HEIGHT - layout.h);
}

void entity::Resize(int w, int h){
	if(w < 0) w = 0;
	if(h < 0) h = 0;
	layout.w = w;
	layout.h = h;
}

// this takes an entire SDL_Rect but only uses the sizes, not the positions
void entity::Resize(SDL_Rect newLayout){
	if(newLayout.w < 0) newLayout.w = 0;
	if(newLayout.h < 0) newLayout.h = 0;
	layout.w = newLayout.w;
	layout.h = newLayout.h;
}

int entity::X() const{
	return layout.x;
}

int entity::Y() const{
	return layout.y;
}

int entity::W() const{
	return layout.w;
}

int entity::H() const{
	return layout.h;
}

int entity::LeftEdge() const{
	return layout.x;
}

int entity::RightEdge() const{
	return layout.x + layout.w - 1;
}

int entity::TopEdge() const{
	return layout.y;
}

int entity::BottomEdge() const{
	return layout.y + layout.h - 1;
}

bool entity::InsideQ(const int x, const int y) const{
	if(x < layout.x || y < layout.y || x >= layout.x + layout.w || 
			y >= layout.y + layout.h) return false;
	return true;
}
