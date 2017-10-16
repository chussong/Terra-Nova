#include "gfxobject.hpp"

namespace TerraNova {

GFXObject::GFXObject(SDL_Renderer* ren, const std::string& spriteFile,
		const int x, const int y, const bool selectable): ren(ren),
		selectable(selectable) {
	layout.x = x;
	layout.y = y;
	ChangeSprite(spriteFile);
}

void GFXObject::ChangeSprite(const std::string& spriteName){
	sprite = GFXManager::RequestSprite(spriteName);
	sprite->MakeDefaultSize(layout);
	if(selectable){
		selectedSprite = GFXManager::RequestSprite(spriteName + "_selected");
	}
}

void GFXObject::Render() const{
	if(!visible) return;
	if(selected){
		selectedSprite->RenderTo(ren, layout);
	} else {
		sprite->RenderTo(ren, layout);
	}
}

int GFXObject::Select(){
	if(selectable){
		selected = true;
	}
	return static_cast<int>(SELECTED);
}

void GFXObject::Deselect(){
	selected = false;
}

std::string GFXObject::HoverText() const{
	return "This object has no specialized hover text.";
}

void GFXObject::MoveTo(int x, int y){
	/*if(x < 0) x = 0;
	if(y < 0) y = 0;
	layout.x = std::min(x, SCREEN_WIDTH - layout.w);
	layout.y = std::min(y, SCREEN_HEIGHT - layout.h);*/
	layout.x = x;
	layout.y = y;
}

// this takes an entire SDL_Rect but only uses the positions, not the sizes
void GFXObject::MoveTo(SDL_Rect newLayout){
	/*if(newLayout.x < 0) newLayout.x = 0;
	if(newLayout.y < 0) newLayout.y = 0;
	layout.x = std::min(newLayout.x, SCREEN_WIDTH - layout.w);
	layout.y = std::min(newLayout.y, SCREEN_HEIGHT - layout.h);*/
	MoveTo(newLayout.x, newLayout.y);
}

void GFXObject::Resize(int w, int h){
	if(w < 0) w = 0;
	if(h < 0) h = 0;
	layout.w = w;
	layout.h = h;
}

// this takes an entire SDL_Rect but only uses the sizes, not the positions
void GFXObject::Resize(SDL_Rect newLayout){
	/*if(newLayout.w < 0) newLayout.w = 0;
	if(newLayout.h < 0) newLayout.h = 0;
	layout.w = newLayout.w;
	layout.h = newLayout.h;*/
	Resize(newLayout.w, newLayout.h);
}

int GFXObject::X() const{
	return layout.x;
}

int GFXObject::Y() const{
	return layout.y;
}

int GFXObject::W() const{
	return layout.w;
}

int GFXObject::H() const{
	return layout.h;
}

const SDL_Rect& GFXObject::Layout() const{
	return layout;
}

int GFXObject::LeftEdge() const{
	return layout.x;
}

int GFXObject::RightEdge() const{
	return layout.x + layout.w - 1;
}

int GFXObject::TopEdge() const{
	return layout.y;
}

int GFXObject::BottomEdge() const{
	return layout.y + layout.h - 1;
}

bool GFXObject::InsideQ(const int x, const int y){
	if(!visible) return false;
	if(x < layout.x || y < layout.y || x >= layout.x + layout.w || 
			y >= layout.y + layout.h) return false;
	return true;
}

} // namespace TerraNova
