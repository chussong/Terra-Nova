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
	SDL_Color colorCode = gfxObject::SDLifyTextColor(color);
	/*switch(color){
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
	}*/
/*	std::cout << "Text \"" << text << "\" set at position (" << textLayout.x 
		<< "," << textLayout.y << ")." << std::endl;*/
	textSprite = std::make_unique<gfxObject>(ren, text, textLayout, colorCode, font);
	if(!textSprite) std::cout << "Error constructing textSprite!" << std::endl;
	textSprite->MakeDefaultSize(textLayout);
	textLayout.x -= textLayout.w/2;
	textLayout.y -= textLayout.h/2;
}

// WARNING: you have to make sure the source outlives this UI element!
void uiElement::AddDynamicText(const int& source, const int x, const int y, 
		TTF_Font* font, const textcolor_t color){
	dynamicTextLayout.x = layout.x + x;
	dynamicTextLayout.y = layout.y + y;
	dynamicTextLayout.w = 0;
	dynamicTextLayout.h = 0;
	SetDynamicText(source, font, color);
}

// WARNING: you have to make sure the source outlives this UI element!
void uiElement::SetDynamicText(const int& source, TTF_Font* font,
		const textcolor_t color){
	if(font == nullptr) font = gfxObject::defaultFont;
	dynamicTextSource = &source;
	dynamicTextCached = source;
	dynamicTextFont = font;
	dynamicTextColor = gfxObject::SDLifyTextColor(color);
	dynamicTextSprite = std::make_unique<gfxObject>(ren, std::to_string(source),
			dynamicTextLayout, dynamicTextColor, font);
	dynamicTextSprite->MakeDefaultSize(dynamicTextLayout);
	dynamicTextLayout.x -= dynamicTextLayout.w/2;
	dynamicTextLayout.y -= dynamicTextLayout.h/2;
}

void uiElement::UpdateDynamicText() const{
	if(*dynamicTextSource != dynamicTextCached){
		dynamicTextCached = *dynamicTextSource;
		dynamicTextSprite = std::make_unique<gfxObject>(ren, 
				std::to_string(dynamicTextCached), dynamicTextLayout,
				dynamicTextColor, dynamicTextFont);
		dynamicTextLayout.x += dynamicTextLayout.w/2;
		dynamicTextLayout.y += dynamicTextLayout.h/2;
		dynamicTextSprite->MakeDefaultSize(dynamicTextLayout);
		dynamicTextLayout.x -= dynamicTextLayout.w/2;
		dynamicTextLayout.y -= dynamicTextLayout.h/2;
	}
}

/*void uiElement::EnableButton(const button_t type){
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
}*/

void uiElement::Render() const {
	if(!visible) return;
	sprite->RenderTo(ren, layout);
	if(textSprite){
		textSprite->RenderTo(ren, textLayout);
//		std::cout << "Rendering an object with text." << std::endl;
	} else {
//		std::cout << "Rendering an object without text." << std::endl;
	}
	if(dynamicTextSprite){
		UpdateDynamicText();
		dynamicTextSprite->RenderTo(ren, dynamicTextLayout);
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

UnitInfoPanel::UnitInfoPanel(SDL_Renderer* ren, const person* unit):
	UIAggregate(ren, SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH, 
			SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT) {
	int panelX = SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH;
	int panelY = SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT;
	background = std::make_unique<uiElement>(ren,
			"unit_info_panel",
			panelX, panelY);
	portrait = std::make_unique<uiElement>(ren,
			unit->Spec()->Name() + "_portrait",
			panelX + PORTRAIT_X, panelY + PORTRAIT_Y);
	factionIcon = std::make_unique<uiElement>(ren,
			"factioncolor_p" + std::to_string(unit->Faction()),
			panelX + FACTIONCOLOR_X, panelY + FACTIONCOLOR_Y);
	factionIcon->AddText(unit->Name(), UNIT_NAME_X, UNIT_NAME_Y);
	healthIcon = std::make_unique<uiElement>(ren,
			"healthicon_" + unit->Species(),
			panelX + HEALTHICON_X, panelY + HEALTHICON_Y);
	healthIcon->AddText(
			std::to_string(unit->Health()) + "/" + std::to_string(unit->MaxHealth()), 
			UNIT_HEALTH_X, UNIT_HEALTH_Y);
	if(unit->Spec()->Attack(0)){
		std::string attackName = unit->Spec()->Attack(0)->Name();
		std::replace(attackName.begin(), attackName.end(), ' ', '_');
		attackIcon = std::make_unique<uiElement>(ren,
				attackName + "_icon",
				panelX + WEAPONICON_X, panelY + WEAPONICON_Y);
		attackIcon->AddText(std::to_string(
				static_cast<int>(std::floor(100*unit->Accuracy()))) + "% | " 
				+ std::to_string(unit->AttackRate()) + "x "
				+ std::to_string(unit->Damage()) + " "
				+ unit->DamageType(),
				UNIT_ATTACK_X, UNIT_ATTACK_Y);
	} else {
		attackIcon = std::make_unique<uiElement>(ren,
				"null_attack_icon",
				panelX + WEAPONICON_X, panelY + WEAPONICON_Y);
		attackIcon->AddText("Unarmed", UNIT_ATTACK_X, UNIT_ATTACK_Y);
	}
}


void UnitInfoPanel::Render() const{
	background->Render();
	portrait->Render();
	factionIcon->Render();
	healthIcon->Render();
	attackIcon->Render();
}

void UnitInfoPanel::UpdateHealth(const person* unit){
	healthIcon->SetText(
			std::to_string(unit->Health()) + "/" + std::to_string(unit->MaxHealth()));
}

UnitOrderPanel::UnitOrderPanel(SDL_Renderer* ren, const person* unit):
	UIAggregate(ren, 0, SCREEN_HEIGHT - UNIT_ORDER_PANEL_HEIGHT),
	background(ren, "unit_order_panel", x, y) {
	UpdatePanel(unit);
}

void UnitOrderPanel::Render() const{
	background.Render();
	for(auto& button : buttons) button.Render();
}

void UnitOrderPanel::UpdatePanel(const person* unit){
	if(!unit){
		buttons.clear();
	} else {
		for(auto order : unit->AvailableOrders()){
			int buttonX = x + 5 + (ORDER_BUTTON_WIDTH+5)*buttons.size()%3;
			int buttonY = y + 5 + (ORDER_BUTTON_HEIGHT+5)*buttons.size()/3;
			buttons.emplace_back(ren, order.name, buttonX, buttonY, order.func);
		}
	}
}
