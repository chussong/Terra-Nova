#include "person.hpp"

std::string person::GenerateGivenName(){
	return std::string("Urist");
}

std::string person::GenerateSurname(){
	return std::string("McColonist");
}

person::person(SDL_Renderer* ren, const std::string& spriteFile, const int x,
		const int y): entity(ren, spriteFile, x, y){
	givenName = GenerateGivenName();
	surname = GenerateSurname();
	srand(time(NULL));
	female = std::rand() % 2;
	spec = "Colonist";
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

void person::ChangeSpec(const std::string spec){
	this->spec = spec;
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

std::string person::Spec() const{
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
	layout.x = newLoc->X() + (newLoc->W() - layout.w)/2;
	layout.y = newLoc->Y() + (newLoc->H() - layout.w)/2;
	location = newLoc;
}

std::shared_ptr<tile> person::Location() const{
	return location;
}
