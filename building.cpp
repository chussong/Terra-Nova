#include "building.hpp"

int buildingType::ID() const{
	return id;
}

std::string buildingType::Name() const{
	return name;
}

std::array<int, LAST_RESOURCE> buildingType::Cost() const{
	return cost;
}

int buildingType::BuildTime() const{
	return buildTime;
}

void buildingType::SetAllowedTerrain(const std::vector<std::shared_ptr<tileType>>& val){
	std::vector<std::weak_ptr<tileType>> weakVal;
	weakVal.resize(val.size());
	for(unsigned int i = 0; i < weakVal.size(); ++i) weakVal[i] = val[i];
	allowedTerrain = weakVal;
}

// this is obviously a very stupid implementation
std::vector<std::shared_ptr<tileType>> buildingType::AllowedTerrain() const{
	std::vector<std::shared_ptr<tileType>> strongTerrain;
	strongTerrain.resize(allowedTerrain.size());
	for(auto i = 0u; i < strongTerrain.size(); ++i){
		strongTerrain[i] = allowedTerrain[i].lock();
	}
	return strongTerrain;
}

void buildingType::SetCanHarvest(const bool val){
	canHarvest = val;
}

bool buildingType::CanHarvest() const{
	return canHarvest;
}

void buildingType::SetAutomatic(const bool val){
	automatic = val;
}

bool buildingType::Automatic() const{
	return automatic;
}

void buildingType::SetMaxOccupants(const int val){
	maxOccupants = val;
}

unsigned int buildingType::MaxOccupants() const{
	return maxOccupants;
}

void buildingType::SetBonusResources(const std::array<int, LAST_RESOURCE>& val){
	bonusResources = val;
}

std::array<int, LAST_RESOURCE> buildingType::BonusResources() const{
	return bonusResources;
}

void buildingType::SetCanTrain(const std::vector<std::shared_ptr<unitType>>& val){
	canTrain = val;
}

void buildingType::SetCanTrain(const std::shared_ptr<unitType> val){
	canTrain.clear();
	canTrain.push_back(val);
}

std::vector<std::shared_ptr<unitType>> buildingType::CanTrain() const{
	return canTrain;
}

std::string buildingPrototype::Name() const{
	return type->Name();
}

std::array<int, LAST_RESOURCE> buildingPrototype::Cost() const{
	return type->Cost();
}

int buildingPrototype::BuildTime() const{
	return type->BuildTime();
}

std::shared_ptr<buildingType> buildingPrototype::Type() const{
	return type;
}

std::string building::Name() const{
	return type->Name();
}

std::array<int, LAST_RESOURCE> building::Cost() const{
	return type->Cost();
}

int building::BuildTime() const{
	return type->BuildTime();
}

void building::StartConstruction(){
	turnsLeft = BuildTime();
}

int building::TurnsLeft() const{
	return turnsLeft;
}

void building::BuildTurn(){
	turnsLeft--;
}

bool building::Finished() const{
	return turnsLeft == 0;
}

bool building::CanHarvest() const{
	return turnsLeft == 0 && type->CanHarvest();
}

bool building::Automatic() const{
	return type->Automatic();
}

unsigned int building::MaxOccupants() const{
	return type->MaxOccupants();
}

std::array<int, LAST_RESOURCE> building::BonusResources() const{
	return type->BonusResources();
}

std::vector<std::shared_ptr<unitType>> building::CanTrain() const{
	return type->CanTrain();
}

void building::StartTraining(std::shared_ptr<unitType> newSpec){
	if(!newSpec){
		std::cerr << "Error: a building was assigned to respec a unit to a "
			<< "blank specialization." << std::endl;
		return;
	}
	nowTraining = newSpec;
	turnsToTrain = newSpec->TrainingTime();
}

int building::TurnsToTrain() const{
	return turnsToTrain;
}

std::shared_ptr<unitType> building::NowTraining() const{
	return nowTraining;
}

void building::TrainingTurn(){
	turnsToTrain--;
}

void building::FinishTraining(){
	nowTraining.reset();
}
