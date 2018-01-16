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
