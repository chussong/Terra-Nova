#include "person.hpp"

std::string person::GenerateGivenName(){
	return std::string("Urist");
}

std::string person::GenerateSurname(){
	return std::string("McColonist");
}

person::person(SDL_Renderer* ren, const std::shared_ptr<unitSpec> spec, 
		const int x, const int y): 
		entity(ren, spec->Name(), x, y, true),
		spec(spec){
	givenName = GenerateGivenName();
	surname = GenerateSurname();
	srand(time(NULL));
	female = std::rand() % 2;
	maxHealth = 100;
	health = 100;
}

void person::ChangeName(const std::string givenName, const std::string surname){
	this->givenName = givenName;
	this->surname = surname;
}

void person::ChangeGender(const std::string gender){
	if(gender == "female") female = true;
	if(gender == "male") female = false;
}

void person::ChangeSpec(const std::shared_ptr<unitSpec> spec){
	this->spec = spec;
	sprite = gfxManager::RequestSprite(spec->Name());
	selectedSprite = gfxManager::RequestSprite(spec->Name() + "_selected");
}

void person::ChangeMaxHealth(const int maxHealth){
	this->maxHealth = maxHealth;
	if(health > maxHealth) health = maxHealth;
}

bool person::TakeDamage(const int damage){
	if(damage >= health){
		health = 0;
		return true;
	}
	health -= damage;
	if(health > maxHealth) health = maxHealth;
	return false;
}

void person::AddItem(const std::string item){
	inventory.push_back(item);
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

std::shared_ptr<unitSpec> person::Spec() const{
	return spec;
}

int person::MaxHealth() const{
	return maxHealth;
}

int person::Health() const{
	return health;
}

std::vector<std::string> person::Inventory() const{
	return inventory;
}

void person::SetLocation(std::shared_ptr<tile> newLoc){
	if(!newLoc){
		std::cerr << "Error: a person tried to move a null tile." << std::endl;
		return;
	}
	if(location) location->RemoveOccupant(shared_from_base<person>());
	newLoc->AddOccupant(shared_from_base<person>());
	layout.x = newLoc->X() + (newLoc->W() - layout.w)/2;
	layout.y = newLoc->Y() + (newLoc->H() - layout.w)/2;
	location = newLoc;
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
