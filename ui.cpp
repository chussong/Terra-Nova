#include "ui.hpp"

namespace TerraNova {

/*void UIElement::AddValue(const int val){
	values.push_back(val);
}*/

/*void UIElement::SetValue(const unsigned int entry, const int newVal){
	if(entry >= values.size()){
		std::cout << "Attempted to set a UI element's invalid value " << entry
			<< " to " << newVal << "." << std::endl;
	} else {
		values[entry] = newVal;
	}
}*/

/*int UIElement::Value(const unsigned int entry) const{
	if(entry >= values.size()){
		std::cout << "Attempted to access a UI element's invalid value "
			<< entry << "." << std::endl;
	} else {
		return values[entry];
	}
	return 0;
}*/

void UIElement::AddText(const std::string& text, const SDL_Rect newLayout,
				TTF_Font* font, const textcolor_t color){
	textLayout = newLayout;
	textLayout.x += layout.x;
	textLayout.y += layout.y;
	if(textLayout.w == 0) textLayout.w = layout.w;
	if(textLayout.h == 0) textLayout.h = layout.h;
	textLayout.x -= textLayout.w/2;
	textLayout.y -= textLayout.h/2;
	SetText(text, font, color);
/*	std::cout << "Text \"" << text << "\" added at position (" << textLayout.x 
		<< "," << textLayout.y << ")." << std::endl;*/
}

void UIElement::SetText(const std::string& text, TTF_Font* font, 
		const textcolor_t color){
	if(font == nullptr) font = defaultFont;
	SDL_Color colorCode = Sprite::SDLifyTextColor(color);
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
	textLayout.x += textLayout.w/2;
	textLayout.y += textLayout.h/2;
	// setting width and height so textSprite knows how wide the box is
	//if(textLayout.w == 0) textLayout.w = layout.w;
	//if(textLayout.h == 0) textLayout.h = layout.h;
	textSprite = std::make_unique<Sprite>(ren, text, textLayout, colorCode, font);
	if(!textSprite) std::cout << "Error constructing textSprite!" << std::endl;
	textSprite->MakeDefaultSize(textLayout);
	textLayout.x -= textLayout.w/2;
	textLayout.y -= textLayout.h/2;
}

// WARNING: you have to make sure the source outlives this UI element!
void UIElement::AddDynamicText(const int& source, const int x, const int y, 
		TTF_Font* font, const textcolor_t color){
	dynamicTextLayout.x = layout.x + x;
	dynamicTextLayout.y = layout.y + y;
	dynamicTextLayout.w = 0;
	dynamicTextLayout.h = 0;
	SetDynamicText(source, font, color);
}

// WARNING: you have to make sure the source outlives this UI element!
void UIElement::SetDynamicText(const int& source, TTF_Font* font,
		const textcolor_t color){
	if(font == nullptr) font = defaultFont;
	dynamicTextSource = &source;
	dynamicTextCached = source;
	dynamicTextFont = font;
	dynamicTextColor = Sprite::SDLifyTextColor(color);
	dynamicTextSprite = std::make_unique<Sprite>(ren, std::to_string(source),
			dynamicTextLayout, dynamicTextColor, font);
	dynamicTextSprite->MakeDefaultSize(dynamicTextLayout);
	dynamicTextLayout.x -= dynamicTextLayout.w/2;
	dynamicTextLayout.y -= dynamicTextLayout.h/2;
}

void UIElement::UpdateDynamicText() const{
	if(*dynamicTextSource != dynamicTextCached){
		dynamicTextCached = *dynamicTextSource;
		dynamicTextSprite = std::make_unique<Sprite>(ren, 
				std::to_string(dynamicTextCached), dynamicTextLayout,
				dynamicTextColor, dynamicTextFont);
		dynamicTextLayout.x += dynamicTextLayout.w/2;
		dynamicTextLayout.y += dynamicTextLayout.h/2;
		dynamicTextSprite->MakeDefaultSize(dynamicTextLayout);
		dynamicTextLayout.x -= dynamicTextLayout.w/2;
		dynamicTextLayout.y -= dynamicTextLayout.h/2;
	}
}

/*void UIElement::EnableButton(const button_t type){
	button = true;
	this->type = type;
}

void UIElement::DisableButton(){
	button = false;
}

int UIElement::Select(){
	if(button){
		switch(type){
			case END_TURN:		return static_cast<int>(NEXT_TURN);
			case BUILDING:		if(values.size() < 1){
									std::cerr << "Error: Building button not \
										given an ID." << std::endl;
									return static_cast<int>(ERROR);
								}
								return static_cast<int>(TRY_BUILD) + values[0];
			case LEAVE_COLONY:	return static_cast<int>(SCREEN_CHANGE);
		}
	}
	return static_cast<int>(ERROR);
}*/

void UIElement::Render() const {
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

void UIElement::MoveTo(int x, int y){
	if(textSprite){
		textLayout.x += x - layout.x;
		textLayout.y += y - layout.y;
	}
	layout.x = x;
	layout.y = y;
}

void UIElement::MoveTo(SDL_Rect newLayout){
	MoveTo(newLayout.x, newLayout.y);
}

//UIAggregate::~UIAggregate() {}
void UIAggregate::Render() const {
	std::cerr << "UIAggregate::Render() shouldn't run!" << std::endl;
}

void InfoPanel::Render() const {
}

void InfoPanel::UpdateFromSource(InfoPanel& toUpdate, const GFXObject& source) {
	// if it can be updated without reconstructing it, do that
	if (toUpdate.Update(source)) return;

	// otherwise, we have to make a new one and swap it in
	std::unique_ptr<InfoPanel> newPanel;
	if (source.IsUnit()) {
		newPanel = std::make_unique<UnitInfoPanel>(toUpdate.Renderer(), 
				dynamic_cast<const Unit&>(source) );
	} else if (source.IsBuilding()) {
		newPanel = std::make_unique<BuildingInfoPanel>(toUpdate.Renderer(),
				dynamic_cast<const Building&>(source) );
	} else if (source.IsBuildingPrototype()) {
		newPanel = std::make_unique<BuildingInfoPanel>(toUpdate.Renderer(),
				dynamic_cast<const BuildingPrototype&>(source) );
	}
	//std::swap(*newPanel, toUpdate);
}

// this just returns false because if the base function gets called it means 
// that the panel definitely needs to be reconstructed
bool InfoPanel::Update(const GFXObject&) {
	std::cerr << "Warning: base InfoPanel::Update called. How?" << std::endl;
	return false;
}

//InfoPanel::~InfoPanel() {}

UnitInfoPanel::UnitInfoPanel(SDL_Renderer* ren, const Unit& source):
	InfoPanel(ren, SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH, 
			SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT) {
	int panelX = SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH;
	int panelY = SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT;
	background = std::make_unique<UIElement>(ren,
			"unit_info_panel",
			panelX, panelY);
	UpdateFromUnit(source);
}

void UnitInfoPanel::Render() const{
	background->Render();
	portrait->Render();
	factionIcon->Render();
	healthIcon->Render();
	attackIcon->Render();
}

// return true if update was successful; return false if unsuccessful, either
// from having a null pointer or needing to change the type of the object
bool UnitInfoPanel::Update(const GFXObject& source) {
	if (source.IsUnit()) {
		UpdateFromUnit(dynamic_cast<const Unit&>(source));
		return true;
	} else {
		InfoPanel::Update(source);
		return false;
	}
}

void UnitInfoPanel::UpdateFromUnit(const Unit& source) {
	int panelX = SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH;
	int panelY = SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT;
	File::Path portraitPath("units");
	portraitPath = portraitPath / source.Spec()->PathName() / "portrait";
	portrait = std::make_unique<UIElement>(ren, portraitPath.string(),
			panelX + PORTRAIT_X, panelY + PORTRAIT_Y);
	factionIcon = std::make_unique<UIElement>(ren,
			"factioncolor_p" + std::to_string(source.Faction()),
			panelX + FACTIONCOLOR_X, panelY + FACTIONCOLOR_Y);
	SDL_Rect textBox;
	textBox.x = UNIT_NAME_X;
	textBox.y = UNIT_NAME_Y;
	textBox.w = UNIT_NAME_W;
	textBox.h = UNIT_NAME_H;
	factionIcon->AddText(source.Name(), textBox);
	if(healthIcon){
		UpdateHealth(source);
	} else {
		healthIcon = std::make_unique<UIElement>(ren,
				"healthicon_" + source.Species(),
				panelX + HEALTHICON_X, panelY + HEALTHICON_Y);
		textBox.x = UNIT_HEALTH_X;
		textBox.y = UNIT_HEALTH_Y;
		textBox.w = UNIT_HEALTH_W;
		textBox.h = UNIT_HEALTH_H;
		healthIcon->AddText(
				std::to_string(source.Health()) + "/" + 
						std::to_string(source.MaxHealth()), textBox);
	}
	textBox.x = UNIT_ATTACK_X;
	textBox.y = UNIT_ATTACK_Y;
	textBox.w = UNIT_ATTACK_W+10;
	textBox.h = UNIT_ATTACK_H;
	if(source.Spec()->Attack(0)){
		std::string attackName = source.Spec()->Attack(0)->Name();
		std::replace(attackName.begin(), attackName.end(), ' ', '_');
		attackIcon = std::make_unique<UIElement>(ren,
				attackName + "_icon",
				panelX + WEAPONICON_X, panelY + WEAPONICON_Y);
		attackIcon->AddText(std::to_string(
				static_cast<int>(std::floor(100*source.Accuracy()))) + "% | " 
				+ std::to_string(source.AttackRate()) + "x "
				+ std::to_string(source.Damage()) + " "
				+ source.DamageType(), textBox);
	} else {
		attackIcon = std::make_unique<UIElement>(ren,
				"null_attack_icon",
				panelX + WEAPONICON_X, panelY + WEAPONICON_Y);
		attackIcon->AddText("Unarmed", textBox);
	}
}

void UnitInfoPanel::UpdateHealth(const Unit& source){
	healthIcon->SetText(
			std::to_string(source.Health()) + "/" 
			+ std::to_string(source.MaxHealth()) );
}

void UnitInfoPanel::UpdateHealthFromSource(UnitInfoPanel& toUpdate, 
		const Unit& source) {
	toUpdate.UpdateHealth(source);
}

//UnitInfoPanel::~UnitInfoPanel() {}

BuildingInfoPanel::BuildingInfoPanel(SDL_Renderer* ren, const Building& source):
	InfoPanel(ren, SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH,
			SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT) {
	// portrait, name, owner, power use, cost
	std::array<int,2> panelCoords = {{SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH,
		SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT}};
	background = MakeBackground(ren, panelCoords);
	portrait = MakePortrait(ren, source.PathName(), panelCoords);
	factionIcon = MakeFactionIcon(ren, source.Faction(), panelCoords);
	powerIcon = MakePowerIcon(ren, source, panelCoords);
	costIcons = MakeCostIcons(ren, source.Cost(), panelCoords);
}

BuildingInfoPanel::BuildingInfoPanel(SDL_Renderer* ren, 
		const BuildingPrototype& source):
	InfoPanel(ren, SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH,
			SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT) {
	// portrait, name, owner, power use, cost
	std::array<int,2> panelCoords = {{SCREEN_WIDTH - UNIT_INFO_PANEL_WIDTH,
		SCREEN_HEIGHT - UNIT_INFO_PANEL_HEIGHT}};
	background = MakeBackground(ren, panelCoords);
	portrait = MakePortrait(ren, source.PathName(), panelCoords);
	factionIcon = MakeFactionIcon(ren, NO_FACTION, panelCoords);
	powerIcon = MakePowerIcon(ren, source, panelCoords);
	costIcons = MakeCostIcons(ren, source.Cost(), panelCoords);
}

std::unique_ptr<UIElement> BuildingInfoPanel::MakeBackground(SDL_Renderer* ren, 
		const std::array<int,2>& panelCoords) {
	return std::make_unique<UIElement>(ren, "info_panel_building", 
			panelCoords[0], panelCoords[1]);
}

std::unique_ptr<UIElement> BuildingInfoPanel::MakePortrait(SDL_Renderer* ren, 
		const std::string& name, const std::array<int,2>& panelCoords) {
	std::unique_ptr<UIElement> ret;
	int xOffset = 5;
	int yOffset = 5;
	File::Path portraitPath("buildings");
	portraitPath = portraitPath / name / "portrait";
	ret = std::make_unique<UIElement>(ren, portraitPath.string(),
			panelCoords[0] + xOffset, panelCoords[1] + yOffset);
	// add name as text to the portrait with some offset
	SDL_Rect textBox = MakeSDLRect(BUILDING_INFO_PORTRAIT_WIDTH + 
			5*BUILDING_INFO_ICON_WIDTH/2, 1 + BUILDING_INFO_ROW_HEIGHT/2,
			5*BUILDING_INFO_ICON_WIDTH, BUILDING_INFO_ROW_HEIGHT);
	ret->AddText(name, textBox);
	return ret;
}

std::unique_ptr<UIElement> BuildingInfoPanel::MakeFactionIcon(SDL_Renderer* ren, 
		char faction, const std::array<int,2>& panelCoords) {
	int xOffset = 5 + BUILDING_INFO_PORTRAIT_WIDTH + 1;
	int yOffset = 5 + BUILDING_INFO_ROW_HEIGHT + 1;
	return std::make_unique<UIElement>(ren, 
			"factioncolor_p" + std::to_string(faction),
			panelCoords[0] + xOffset, panelCoords[1] + yOffset);
}

std::unique_ptr<UIElement> BuildingInfoPanel::MakePowerIcon(SDL_Renderer* ren, 
		const Building& source, const std::array<int,2>& panelCoords) {
	int xOffset = 5 + BUILDING_INFO_PORTRAIT_WIDTH + 2*BUILDING_INFO_ICON_WIDTH/2;
	int yOffset = 5 + 2*BUILDING_INFO_ROW_HEIGHT/2;
	std::unique_ptr<UIElement> ret;
	std::string powerUse = "0";
	std::string iconName = "power_off";
	textcolor_t powerColor = BLACK;
	if (source.PoweredOn()) {
		if (source.PowerProduction() > 0) {
			powerUse = "+" + std::to_string(source.PowerProduction());
			powerColor = BLUE;
		} else {
			powerUse = std::to_string(source.PowerConsumption());
			powerColor = BLACK;
		}
		iconName = "power_on";
	}
	File::Path iconPath("buildings");
	iconPath /= iconName;
	ret = std::make_unique<UIElement>(ren, iconPath.string(),
			panelCoords[0] + xOffset, panelCoords[1] + yOffset);
	// add text showing { poweredOn ? powerConsumption : 0 } & maybe avail. pwr?
	SDL_Rect textBox = MakeSDLRect(3*BUILDING_INFO_ICON_WIDTH/2,
			BUILDING_INFO_ROW_HEIGHT/2, 3*BUILDING_INFO_ICON_WIDTH,
			BUILDING_INFO_ROW_HEIGHT);
	ret->AddText(powerUse, textBox, nullptr, powerColor);
	return ret;
}

std::unique_ptr<UIElement> BuildingInfoPanel::MakePowerIcon(SDL_Renderer* ren, 
		const BuildingPrototype& source, const std::array<int,2>& panelCoords) {
	int xOffset = 5 + BUILDING_INFO_PORTRAIT_WIDTH + 2*BUILDING_INFO_ICON_WIDTH/2;
	int yOffset = 5 + 2*BUILDING_INFO_ROW_HEIGHT/2;
	std::unique_ptr<UIElement> ret;
	File::Path iconPath("buildings");
	iconPath /= "power_on";
	ret = std::make_unique<UIElement>(ren, iconPath.string(),
			panelCoords[0] + xOffset, panelCoords[1] + yOffset);
	// add text showing power consumption and available power
	SDL_Rect textBox = MakeSDLRect(3*BUILDING_INFO_ICON_WIDTH/2,
			BUILDING_INFO_ROW_HEIGHT/2, 3*BUILDING_INFO_ICON_WIDTH,
			BUILDING_INFO_ROW_HEIGHT);
	std::string powerUse;
	textcolor_t powerColor = BLACK;
	if (source.PowerProduction() == 0) {
		powerUse = std::to_string(source.PowerConsumption());
		powerColor = BLACK;
	} else {
		powerUse = "+" + std::to_string(source.PowerProduction());
		powerColor = BLUE;
		// should also change the text color here
	}
	ret->AddText(powerUse, textBox, nullptr, powerColor);
	return ret;
}

std::unique_ptr<std::array<UIElement,LAST_RESOURCE>> BuildingInfoPanel::MakeCostIcons(
		SDL_Renderer* ren, const std::array<int, LAST_RESOURCE>& costs,
		const std::array<int,2>& panelCoords) {
	std::array<std::string, LAST_RESOURCE> resourceNames{{"food_cost", 
		"carbon_cost", "silicon_cost", "iron_cost"}};
	std::array<int, LAST_RESOURCE> xCoords;
	for (auto i = 0u; i < xCoords.size(); ++i) {
		xCoords[i] = panelCoords[0] + 5 + BUILDING_INFO_PORTRAIT_WIDTH + 1 + 
			i*BUILDING_INFO_ICON_WIDTH;
	}
	int yCoord = panelCoords[1] + 5 + 2*BUILDING_INFO_ROW_HEIGHT + 1;
	std::unique_ptr<std::array<UIElement, LAST_RESOURCE>> ret;
	std::array<UIElement,LAST_RESOURCE> contents({{
			UIElement{ren, resourceNames[0], xCoords[0], yCoord},
			UIElement{ren, resourceNames[1], xCoords[1], yCoord},
			UIElement{ren, resourceNames[2], xCoords[2], yCoord},
			UIElement{ren, resourceNames[3], xCoords[3], yCoord}
			}});
	SDL_Rect textBox = MakeSDLRect(BUILDING_INFO_ICON_WIDTH/2, 
			3*BUILDING_INFO_ROW_HEIGHT/5, 
			BUILDING_INFO_ICON_WIDTH, BUILDING_INFO_ROW_HEIGHT);
	for (auto i = 0u; i < LAST_RESOURCE; ++i) {
		contents[i].AddText(std::to_string(costs[i]), textBox);
	}
	ret = std::make_unique<std::array<UIElement, LAST_RESOURCE>>(std::move(contents));
	// add text to each component of ret
	return ret;
}

void BuildingInfoPanel::Render() const {
	background->Render();
	portrait->Render();
	factionIcon->Render();
	powerIcon->Render();
	for (const auto& costIcon : *costIcons) costIcon.Render();
}
//BuildingInfoPanel::~BuildingInfoPanel() {}
bool BuildingInfoPanel::Update(const GFXObject& source) {
	std::cout << "Updating BuildingInfoPanel." << std::endl;
	return false;
}

UnitOrderPanel::UnitOrderPanel(SDL_Renderer* ren, Unit* source): GFXObject(ren,
		File::SpritePath() / "unit_order_panel", 
		0, SCREEN_HEIGHT - ORDER_PANEL_HEIGHT) {
	Update(*source);
	activeButton = -1u;
}

void UnitOrderPanel::Render() const{
	GFXObject::Render();
	for(auto& button : buttons) button.Render();
}

// the source argument is not const because the buttons can make it do stuff
void UnitOrderPanel::Update(Unit& source){
	buttons.clear();
	for(auto order : source.AvailableOrders()){
		int buttonX = X() + 5 + (ORDER_BUTTON_WIDTH+5)*(buttons.size()%3);
		int buttonY = Y() + 5 + (ORDER_BUTTON_HEIGHT+5)*(buttons.size()/3);
		buttons.emplace_back(ren, "button_" + order.name, buttonX, buttonY,
				order.func);
	}
}

void UnitOrderPanel::AddButton(Button&& newButton){
	newButton.MoveTo(X() + 5 + (ORDER_BUTTON_WIDTH+5)*(buttons.size()%3),
			Y() + 5 + (ORDER_BUTTON_HEIGHT+5)*(buttons.size()/3));
	buttons.push_back(std::move(newButton));
}

// Note that this returns false if you click on the frame rather than a button
bool UnitOrderPanel::InsideQ(const int x, const int y){
	for(auto i = 0u; i < buttons.size(); ++i){
		if(buttons[i].InsideQ(x, y)){
			activeButton = i;
			return true;
		}
	}
	return false;
}

bool UnitOrderPanel::Click(){
	if(activeButton >= buttons.size()){
		std::cerr << "Error: UnitOrderPanel clicked, but it does not have an "
			<< "active button set by InsideQ()." << std::endl;
	}
	const bool ret = buttons[activeButton].Click();
	activeButton = -1u;
	return ret;
}

void UnitOrderPanel::MoveTo(int x, int y){
	GFXObject::MoveTo(x,y);
	for(auto i = 0u; i < buttons.size(); ++i){
		buttons[i].MoveTo(x + 5 + (ORDER_BUTTON_WIDTH+5)*(i%3),
				y + 5 + (ORDER_BUTTON_HEIGHT+5)*(i/3));
	}
}

} // namespace TerraNova
