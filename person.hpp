#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
#include "gamevars.hpp"
#include "entity.hpp"
#include "tile.hpp"

class tile;

class unitSpec {
	const int id;
	const std::string name;
	
	const int maxHealth;

	const int trainingTime;

	bool canRespec = true;

	public:
		unitSpec() = delete;
		unitSpec(const int id, const std::string& name, const int maxHealth,
			const int trainingTime): id(id), name(name), maxHealth(maxHealth), 
			trainingTime(trainingTime) {}

		int ID()			const			{return id;}
		std::string Name()	const			{return name;}
		int MaxHealth()		const			{return maxHealth;}
		int TrainingTime()	const			{return trainingTime;}

		bool CanRespec()	const 			{return canRespec;}
		void SetCanRespec(const bool val)	{canRespec = val;}

};

class person : public entity {
	std::string givenName;
	std::string surname;
	bool female;
	std::shared_ptr<unitSpec> spec;
	
	int maxHealth;
	int health;
	std::vector<std::string> inventory;

	std::shared_ptr<tile> location;

	static std::string GenerateGivenName();
	static std::string GenerateSurname();

	public:
		person() = delete;
		person(SDL_Renderer* ren, const std::shared_ptr<unitSpec> spec, const int x,
				const int y);
		person(const person& other) = delete;
		person& operator=(const person other) = delete;

		void ChangeName(const std::string givenName, const std::string surname);
		void ChangeGender(const std::string gender);
		void ChangeSpec(const std::shared_ptr<unitSpec> spec);
		void ChangeMaxHealth(const int maxHealth);
		bool TakeDamage(const int damage); // false = survived
		void AddItem(const std::string item);

		std::string					Name()		const;
		std::string 				GivenName() const;
		std::string 				Surname() 	const;
		std::string					Gender()	const;
		std::string					NomPronoun()const;
		std::string					NomPronCap()const;
		std::string					AccPronoun()const;
		std::string					AccPronCap()const;
		std::string					PosPronoun()const;
		std::string					PosPronCap()const;
		std::shared_ptr<unitSpec>	Spec()	 	const;
		int							MaxHealth() const;
		int							Health()	const;
		std::vector<std::string> 	Inventory() const;

		void SetLocation(std::shared_ptr<tile> newLoc);
		std::shared_ptr<tile> Location() const;
		int Row() const;
		int Colm() const;

		bool CanRespec()	const			{return spec->CanRespec();}
};

#endif
