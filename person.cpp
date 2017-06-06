#include "person.hpp"

std::string person::GenerateGivenName(){
	return std::string("Urist");
}

std::string person::GenerateSurname(){
	return std::string("McColonist");
}

person::person(SDL_Renderer* ren, 
		const std::shared_ptr<unitType> spec, const char faction): 
		entity(ren, spec->Name(), 0, 0, true),
		spec(spec), faction(faction){
	givenName = GenerateGivenName();
	surname = GenerateSurname();
	srand(time(NULL));
	female = std::rand() % 2;
	health = MaxHealth();
	movesLeft = MoveSpeed()+1;

	healthBackground = gfxManager::RequestSprite("healthbar_background");
	healthBar = gfxManager::RequestSprite("healthbar_p" + std::to_string(faction));
}

void person::ChangeName(const std::string givenName, const std::string surname){
	this->givenName = givenName;
	this->surname = surname;
}

void person::ChangeGender(const std::string gender){
	if(gender == "female") female = true;
	if(gender == "male") female = false;
}

void person::ChangeSpec(const std::shared_ptr<unitType> spec){
	this->spec = spec;
	sprite = gfxManager::RequestSprite(spec->Name());
	selectedSprite = gfxManager::RequestSprite(spec->Name() + "_selected");
}

bool person::TakeDamage(const int damage){
	if(damage >= health){
		health = 0;
		Die();
		return true;
	}
	health -= damage;
	if(health > MaxHealth()) health = MaxHealth();
	return false;
}

void person::Die(){
	std::cout << Name() << " belonging to player " << (int)faction << " has died!" 
		<< std::endl;
	if(location) location->RemoveOccupant(shared_from_base<person>());
	location.reset();
}

void person::AddItem(const std::string item){
	inventory.push_back(item);
}

void person::Render() const{
	//std::cout << Dead() << std::endl;
	if(Dead()) return;
	entity::Render();
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
}

void person::ProcessTurn(){
	movesLeft = MoveSpeed();
}

std::string person::Name() const{
	std::string name(givenName);
	name.append(" ");
	name.append(surname);
	return name;
}

std::string person::GivenName() const{
	return givenName;
}

std::string person::Surname() const{
	return surname;
}

std::string person::Species() const{
	return "human";
}

std::string person::Gender() const{
	if(female) return std::string("female");
	return std::string("male");
}

std::string person::NomPronoun() const{
	if(female) return std::string("she");
	return std::string("he");
}

std::string person::NomPronCap() const{
	if(female) return std::string("She");
	return std::string("He");
}

std::string person::AccPronoun() const{
	if(female) return std::string("her");
	return std::string("him");
}

std::string person::AccPronCap() const{
	if(female) return std::string("Her");
	return std::string("Him");
}

std::string person::PosPronoun() const{
	if(female) return std::string("her");
	return std::string("his");
}

std::string person::PosPronCap() const{
	if(female) return std::string("Her");
	return std::string("His");
}

std::shared_ptr<unitType> person::Spec() const{
	return spec;
}

int person::MaxHealth() const{
	return spec->MaxHealth();
}

int person::Health() const{
	return health;
}

std::vector<std::string> person::Inventory() const{
	return inventory;
}

int person::MoveSpeed()	const{
	return spec->MoveSpeed();
}

int person::MovesLeft()	const{
	return movesLeft;
}

bool person::MoveToTile(std::shared_ptr<tile> newLoc){
	if(!newLoc){
		std::cerr << "Error: a person tried to move a null tile." << std::endl;
		return false;
	}
	if(!newLoc->AddOccupant(shared_from_base<person>())){
		std::cout << "That tile is already fully occupied." << std::endl;
		return false;
	}
	if(location) location->RemoveOccupant(shared_from_base<person>());
	layout.x = MAPDISP_ORIGIN_X + newLoc->X() + (newLoc->W() - layout.w)/2;
	layout.y = MAPDISP_ORIGIN_Y + newLoc->Y() + (newLoc->H() - layout.w)/2;
	--movesLeft;
	location = newLoc;
	return true;
}

std::shared_ptr<tile> person::Location() const{
	return location;
}

int person::Row() const{
	return location->Row();
}

int person::Colm() const{
	return location->Colm();
}

std::vector<std::shared_ptr<attackType>> person::Attacks() const{
	return spec->Attacks();
}

int person::Damage(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->Damage();
	return 0;
}

float person::Accuracy(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->Accuracy();
	return 0;
}

int person::AttackRate(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->AttackRate();
	return 0;
}

std::string person::DamageType(const unsigned int num) const{
	if(num < Attacks().size()) return Attacks()[num]->DamageType();
	return 0;
}

void person::Attack(std::shared_ptr<person>& target) const{
	std::random_device dev;
	std::mt19937 gen(dev());
	std::uniform_real_distribution<> dist(0,1);
	for(auto& atk : Attacks()){
		for(int hit = 0; hit < atk->AttackRate(); ++hit){
			if(dist(gen) <= atk->Accuracy()){
				if(target->TakeDamage(atk->Damage())){
					target.reset();
					return;
				}
			} else {
			}
		}
	}
}

void person::Fight(std::shared_ptr<person> attacker, std::shared_ptr<person> target){
	if(!attacker){
		std::cerr << "Error: attempting a fight but there is no attacker." << std::endl;
		return;
	}
	if(!target){
		std::cerr << "Error: attempting a fight but there is no target." << std::endl;
		return;
	}
	if(attacker->MovesLeft() < 1){
		std::cout << "Fighting requires an available movement point." << std::endl;
		return;
	}
	attacker->Attack(target);
	if(target){
		attacker->movesLeft--;
		target->Attack(attacker);
	}
}
