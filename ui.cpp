#include "ui.hpp"

void uiElement::AddValue(const int val){
	values.push_back(val);
}

void uiElement::SetValue(const unsigned int entry, const int newVal){
	if(entry >= values.size()){
		std::cout << "Attempted to set a UI element's invalid value " << entry
			<< " to " << newVal << "." << std::endl;
	} else {
		values[entry] = newVal;
	}
}

int uiElement::Value(const unsigned int entry) const{
	if(entry >= values.size()){
		std::cout << "Attempted to access a UI element's invalid value "
			<< entry << "." << std::endl;
	} else {
		return values[entry];
	}
	return 0;
}

void uiElement::AddText(const std::string& text, const int x, const int y, 
				TTF_Font* font, const textcolor_t color){
	textLayout.x = layout.x + x;
	textLayout.y = layout.y + y;
	textLayout.w = 0;
	textLayout.h = 0;
	SetText(text, font, color);
/*	std::cout << "Text \"" << text << "\" added at position (" << textLayout.x 
		<< "," << textLayout.y << ")." << std::endl;*/
}

void uiElement::SetText(const std::string& text, TTF_Font* font, 
		const textcolor_t color){
	if(font == nullptr) font = gfxObject::defaultFont;
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
	textLayout.x += textLayout.w/2;
	textLayout.y += textLayout.h/2;
/*	std::cout << "Text \"" << text << "\" set at position (" << textLayout.x 
		<< "," << textLayout.y << ")." << std::endl;*/
	textSprite = std::make_unique<gfxObject>(ren, text, textLayout, colorCode, font);
	if(!textSprite) std::cout << "Error constructing textSprite!" << std::endl;
}
	

void uiElement::EnableButton(const button_t type){
	button = true;
	this->type = type;
}

void uiElement::DisableButton(){
	button = false;
}

int uiElement::Select(){
	if(button){
		switch(type){
			case END_TURN:		return static_cast<int>(NEXT_TURN);
			case BUILDING:		if(values.size() < 1){
									std::cerr << "Error: building button not \
										given an ID." << std::endl;
									return static_cast<int>(ERROR);
								}
								return static_cast<int>(TRY_BUILD) + values[0];
			case LEAVE_COLONY:	return static_cast<int>(SCREEN_CHANGE);
		}
	}
	return static_cast<int>(ERROR);
}

void uiElement::Render() const {
	sprite->RenderTo(ren, layout);
	if(textSprite){
		textSprite->RenderTo(ren, textLayout);
//		std::cout << "Rendering an object with text." << std::endl;
	} else {
//		std::cout << "Rendering an object without text." << std::endl;
	}
}

void uiElement::MoveTo(int x, int y){
	if(textSprite){
		textLayout.x += x - layout.x;
		textLayout.y += y - layout.y;
	}
	layout.x = x;
	layout.y = y;
}

void uiElement::MoveTo(SDL_Rect newLayout){
	MoveTo(newLayout.x, newLayout.y);
}

unitInfoPanel::unitInfoPanel(SDL_Renderer* ren, std::shared_ptr<person> unit){
	int panelX = SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH;
	int panelY = SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT;
	background = std::make_shared<uiElement>(ren,
			"unit_info_panel",
			panelX, panelY);
	portrait = std::make_shared<uiElement>(ren,
			unit->Spec()->Name() + "_portrait",
			panelX + PORTRAIT_X, panelY + PORTRAIT_Y);
	factionIcon = std::make_shared<uiElement>(ren,
			"factioncolor_p" + std::to_string(unit->Faction()),
			panelX + FACTIONCOLOR_X, panelY + FACTIONCOLOR_Y);
	factionIcon->AddText(unit->Name(), UNIT_NAME_X, UNIT_NAME_Y);
	healthIcon = std::make_shared<uiElement>(ren,
			"healthicon_" + unit->Species(),
			panelX + HEALTHICON_X, panelY + HEALTHICON_Y);
	healthIcon->AddText(
			std::to_string(unit->Health()) + "/" + std::to_string(unit->MaxHealth()), 
			UNIT_HEALTH_X, UNIT_HEALTH_Y);
	if(unit->Spec()->Attack(0)){
		std::string attackName = unit->Spec()->Attack(0)->Name();
		std::replace(attackName.begin(), attackName.end(), ' ', '_');
		attackIcon = std::make_shared<uiElement>(ren,
				attackName + "_icon",
				panelX + WEAPONICON_X, panelY + WEAPONICON_Y);
		attackIcon->AddText(std::to_string(
				static_cast<int>(std::floor(100*unit->Accuracy()))) + "% | " 
				+ std::to_string(unit->NumAttacks()) + "x "
				+ std::to_string(unit->Damage()) + " "
				+ unit->DamageType(),
				UNIT_ATTACK_X, UNIT_ATTACK_Y);
	} else {
		attackIcon = std::make_shared<uiElement>(ren,
				"null_attack_icon",
				panelX + WEAPONICON_X, panelY + WEAPONICON_Y);
		attackIcon->AddText("Unarmed", UNIT_ATTACK_X, UNIT_ATTACK_Y);
	}
}


void unitInfoPanel::Render(){
	background->Render();
	portrait->Render();
	factionIcon->Render();
	healthIcon->Render();
	attackIcon->Render();
}

void unitInfoPanel::UpdateHealth(const std::shared_ptr<person> unit){
	healthIcon->SetText(
			std::to_string(unit->Health()) + "/" + std::to_string(unit->MaxHealth()));
}
