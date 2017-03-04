#include "ui.hpp"

void uiElement::AddText(const std::string& text, const int x, const int y, 
				TTF_Font* font, const textcolor_t color){
	if(font == nullptr) font = gfxObject::defaultFont;
	textLayout.x = x;
	textLayout.y = y;
	SDL_Color colorCode;
	switch(color){
		case BLACK: 	colorCode.r = 0;
						colorCode.g = 0;
						colorCode.b = 0;
						colorCode.a = 255;
						break;
		default:		colorCode.r = 0;
						colorCode.g = 0;
						colorCode.b = 0;
						colorCode.a = 255;
						break;
	}
	textSprite = std::make_unique<gfxObject>(ren, text, textLayout, colorCode, font);
}

void uiElement::EnableButton(const button_t type){
	button = true;
	this->type = type;
}

void uiElement::DisableButton(){
	button = false;
}

signal_t uiElement::Select(){
	if(button){
		switch(type){
			case END_TURN:		return NEXT_TURN;
		}
	}
	return ERROR;
}

void uiElement::Render() const {
	sprite->RenderTo(ren, layout);
	if(textSprite) textSprite->RenderTo(ren, textLayout);
}
