#include "building.hpp"

int BuildingType::ID() const{
	return id;
}

std::string BuildingType::Name() const{
	return name;
}

std::array<int, LAST_RESOURCE> BuildingType::Cost() const{
	return cost;
}

int BuildingType::BuildTime() const{
	return buildTime;
}

/*void BuildingType::SetAllowedTerrain(const std::vector<std::shared_ptr<TileType>>& val){
	std::vector<std::weak_ptr<TileType>> weakVal;
	weakVal.resize(val.size());
	for(unsigned int i = 0; i < weakVal.size(); ++i) weakVal[i] = val[i];
	allowedTerrain = weakVal;
}

// this is obviously a very stupid implementation
std::vector<std::shared_ptr<TileType>> BuildingType::AllowedTerrain() const{
	std::vector<std::shared_ptr<TileType>> strongTerrain;
	strongTerrain.resize(allowedTerrain.size());
	for(auto i = 0u; i < strongTerrain.size(); ++i){
		strongTerrain[i] = allowedTerrain[i].lock();
	}
	return strongTerrain;
}*/

void BuildingType::SetCanHarvest(const bool val){
	canHarvest = val;
}

bool BuildingType::CanHarvest() const{
	return canHarvest;
}

void BuildingType::SetAutomatic(const bool val){
	automatic = val;
}

bool BuildingType::Automatic() const{
	return automatic;
}

void BuildingType::SetMaxOccupants(const int val){
	maxOccupants = val;
}

unsigned int BuildingType::MaxOccupants() const{
	return maxOccupants;
}

void BuildingType::SetBonusResources(const std::array<int, LAST_RESOURCE>& val){
	bonusResources = val;
}

std::array<int, LAST_RESOURCE> BuildingType::BonusResources() const{
	return bonusResources;
}

void BuildingType::SetCanTrain(const std::vector<std::shared_ptr<UnitType>>& val){
	canTrain = val;
}

void BuildingType::SetCanTrain(const std::shared_ptr<UnitType> val){
	canTrain.clear();
	canTrain.push_back(val);
}

std::vector<std::shared_ptr<UnitType>> BuildingType::CanTrain() const{
	return canTrain;
}

std::string BuildingPrototype::Name() const{
	return type->Name();
}

std::array<int, LAST_RESOURCE> BuildingPrototype::Cost() const{
	return type->Cost();
}

int BuildingPrototype::BuildTime() const{
	return type->BuildTime();
}

const BuildingType* BuildingPrototype::Type() const{
	return type;
}

std::string Building::Name() const{
	return type->Name();
}

std::array<int, LAST_RESOURCE> Building::Cost() const{
	return type->Cost();
}

int Building::BuildTime() const{
	return type->BuildTime();
}

void Building::StartConstruction(){
	turnsLeft = BuildTime();
}

int Building::TurnsLeft() const{
	return turnsLeft;
}

void Building::BuildTurn(){
	turnsLeft--;
}

bool Building::Finished() const{
	return turnsLeft == 0;
}

bool Building::CanHarvest() const{
	return turnsLeft == 0 && type->CanHarvest();
}

bool Building::Automatic() const{
	return type->Automatic();
}

unsigned int Building::MaxOccupants() const{
	return type->MaxOccupants();
}

std::array<int, LAST_RESOURCE> Building::BonusResources() const{
	return type->BonusResources();
}

std::vector<std::shared_ptr<UnitType>> Building::CanTrain() const{
	return type->CanTrain();
}

void Building::StartTraining(std::shared_ptr<UnitType> newSpec){
	if(!newSpec){
		std::cerr << "Error: a Building was assigned to respec a Unit to a "
			<< "blank specialization." << std::endl;
		return;
	}
	nowTraining = newSpec;
	turnsToTrain = newSpec->TrainingTime();
}

int Building::TurnsToTrain() const{
	return turnsToTrain;
}

std::shared_ptr<UnitType> Building::NowTraining() const{
	return nowTraining;
}

void Building::TrainingTurn(){
	turnsToTrain--;
}

void Building::FinishTraining(){
	nowTraining.reset();
}
