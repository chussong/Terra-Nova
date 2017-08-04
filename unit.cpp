#include "unit.hpp"

std::string Unit::GenerateGivenName(){
	return std::string("Urist");
}

std::string Unit::GenerateSurname(){
	return std::string("McColonist");
}

Unit::Unit(SDL_Renderer* ren, 
		const std::shared_ptr<UnitType> spec, const char faction): 
		GFXObject(ren, "units/" + spec->Name() + "/sprite", 0, 0, true),
		spec(spec), faction(faction), location({{-1u,-1u}}) {
	givenName = GenerateGivenName();
	surname = GenerateSurname();
	srand(time(NULL));
	female = std::rand() % 2;
	health = MaxHealth();
	movesLeft = MoveSpeed()+1;

	healthBackground = GFXManager::RequestSprite("healthbar_background");
	healthBar = GFXManager::RequestSprite("healthbar_p" + std::to_string(faction));

	SetOrders(ORDER_PATROL);
}

void Unit::ChangeName(const std::string givenName, const std::string surname){
	this->givenName = givenName;
	this->surname = surname;
}

void Unit::ChangeName(const std::string fullName){
	givenName = fullName.substr(0, fullName.find(' '));
	surname = fullName.substr(fullName.find(' ') + 1);
}

void Unit::ChangeGender(const std::string gender){
	if(gender == "female") female = true;
	if(gender == "male") female = false;
}

void Unit::ChangeSpec(const std::shared_ptr<UnitType> spec){
	this->spec = spec;
	sprite = GFXManager::RequestSprite(spec->Name());
	selectedSprite = GFXManager::RequestSprite(
			"units/" + spec->Name() + "/sprite_selected");
}

bool Unit::TakeDamage(const int damage){
	if(damage >= health){
		health = 0;
		Die();
		return true;
	}
	health -= damage;
	if(health > MaxHealth()) health = MaxHealth();
	return false;
}

// this corresponds to things which consume the unit but don't trigger death effects
void Unit::BeConsumed(){
	health = 0;
	Die(); // should probably be something else that also gets the unit deleted
}

void Unit::Die(){
	std::cout << Name() << " belonging to player " << (int)faction << " has died!" 
		<< std::endl;
	visible = false;
}

void Unit::AddItem(const std::string item){
	inventory.push_back(item);
}

void Unit::MoveSpriteToTile(const int X, const int Y, const int W, const int H){
	MoveTo((MAPDISP_ORIGIN_X + X + (W - layout.w)/2), 
			(MAPDISP_ORIGIN_Y + Y + (H - layout.w)/2));
}

void Unit::Render() const{
	//std::cout << Dead() << std::endl;
	if(!visible) return;
	if(Dead()) return;

	if(myPath) myPath->RenderStartingFrom(X() + PERSON_WIDTH/2,
			Y() - PERSON_HEIGHT/2);

	GFXObject::Render();
	SDL_Rect healthLayout;
	healthLayout.w = HEALTH_BAR_WIDTH;
	healthLayout.h = HEALTH_BAR_HEIGHT;
	healthLayout.x = layout.x + (layout.w - healthLayout.w)/2;
	healthLayout.y = layout.y - healthLayout.h;
	healthBackground->RenderTo(ren, healthLayout);
	healthLayout.x += 1;
	healthLayout.y += 1;
	healthLayout.h -= 2;
	healthLayout.w -= 2;
	healthLayout.w *= Health();
	healthLayout.w /= MaxHealth();
	healthBar->RenderTo(ren, healthLayout);

	if(orderIcon){
		SDL_Rect orderLayout = layout;
		orderIcon->MakeDefaultSize(orderLayout);
		orderLayout.y += layout.h - orderLayout.h;
		orderIcon->RenderTo(orderLayout);
	}
}

void Unit::ProcessTurn(){
	movesLeft = MoveSpeed();
}

std::string Unit::Name() const{
	std::string name(givenName);
	name.append(" ");
	name.append(surname);
	return name;
}

std::string Unit::GivenName() const{
	return givenName;
}

std::string Unit::Surname() const{
	return surname;
}

std::string Unit::Species() const{
	return "human";
}

std::string Unit::Gender() const{
	if(female) return std::string("female");
	return std::string("male");
}

std::string Unit::NomPronoun() const{
	if(female) return std::string("she");
	return std::string("he");
}

std::string Unit::NomPronCap() const{
	if(female) return std::string("She");
	return std::string("He");
}

std::string Unit::AccPronoun() const{
	if(female) return std::string("her");
	return std::string("him");
}

std::string Unit::AccPronCap() const{
	if(female) return std::string("Her");
	return std::string("Him");
}

std::string Unit::PosPronoun() const{
	if(female) return std::string("her");
	return std::string("his");
}

std::string Unit::PosPronCap() const{
	if(female) return std::string("Her");
	return std::string("His");
}

std::shared_ptr<UnitType> Unit::Spec() const{
	return spec;
}

int Unit::MaxHealth() const{
	return spec->MaxHealth();
}

int Unit::Health() const{
	return health;
}

std::vector<std::string> Unit::Inventory() const{
	return inventory;
}

int Unit::MoveSpeed()	const{
	return spec->MoveSpeed();
}

int Unit::MovesLeft()	const{
	return movesLeft;
}

void Unit::SetLocation(const unsigned int row, const unsigned int colm, 
		const bool usesMove){
	/*std::cout << "Setting location of " << Name() << " to (" << row << ","
		<< colm << ")." << std::endl;*/
	location[0] = row;
	location[1] = colm;
	if(usesMove) movesLeft--;
}

std::array<unsigned int, 2> Unit::Location() const{
	return location;
}

int Unit::Row() const{
	return location[0];
}

int Unit::Colm() const{
	return location[1];
}

std::array<unsigned int, 2> Unit::NextStep(){
	if(!myPath){
		std::cerr << "Error: asked for the next step along a Path, but the "
			<< "Unit does not have one." << std::endl;
		return std::array<unsigned int, 2>({{-1u, -1u}});
	}
	return myPath->NextStep();
}

bool Unit::AdvancePath(){
	if(!myPath) return true;
	bool arrived(myPath->Advance());
	if(arrived){
		SetOrders(ORDER_PATROL);
		myPath.reset();
	}
	return arrived;
}

void Unit::SetOrders(const order_t newOrders){
	orders = newOrders;
	std::string orderName = "order_";
	switch(newOrders){
		case ORDER_ADVANCE:
			orderName += "move";
			break;
		case ORDER_PATROL:
			orderName += "patrol";
			break;
		case ORDER_HARVEST:
			orderName += "harvest";
			break;
	}
	orderIcon = GFXManager::RequestSprite(orderName);
}

void Unit::OrderMove(std::unique_ptr<Path> newPath){
	if(newPath){
		SetOrders(ORDER_ADVANCE);
		myPath = std::move(newPath);
	} else {
		// empty Path means it's an order to move to the place you already are
		OrderPatrol();
	}
}

void Unit::OrderPatrol(){
	SetOrders(ORDER_PATROL);
	myPath = nullptr;
}

void Unit::OrderHarvest(){
	SetOrders(ORDER_HARVEST);
	myPath = nullptr;
}

std::vector<Order> Unit::AvailableOrders() {
	std::vector<Order> ret;
	ret.emplace_back("patrol", std::bind(&Unit::OrderPatrol, this));
	ret.emplace_back("harvest", std::bind(&Unit::OrderHarvest, this));
	return ret;
}

std::vector<std::shared_ptr<AttackType>> Unit::Attacks() const{
	return spec->Attacks();
}

int Unit::Damage(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->Damage();
	return 0;
}

float Unit::Accuracy(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->Accuracy();
	return 0;
}

int Unit::AttackRate(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->AttackRate();
	return 0;
}

std::string Unit::DamageType(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->DamageType();
	return 0;
}

// return true if target was killed
bool Unit::Attack(Unit* target) const{
	std::random_device dev;
	std::mt19937 gen(dev());
	std::uniform_real_distribution<> dist(0,1);
	for(auto& atk : Attacks()){
		for(int hit = 0; hit < atk->AttackRate(); ++hit){
			if(dist(gen) <= atk->Accuracy()){
				if(target->TakeDamage(atk->Damage())) return true;
			}
		}
	}
	return false;
}

void Unit::Fight(Unit* attacker, Unit* target){
	if(!attacker){
		std::cerr << "Error: attempting a fight but there is no attacker." << std::endl;
		return;
	}
	if(!target){
		std::cerr << "Error: attempting a fight but there is no target." << std::endl;
		return;
	}
	/*if(attacker->MovesLeft() < 1){
		std::cout << "Fighting requires an available movement point." << std::endl;
		return;
	}*/
	bool targetDead = attacker->Attack(target);
	if(!targetDead){
		attacker->movesLeft--;
		target->Attack(attacker);
	}
}
