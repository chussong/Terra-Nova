#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <random>
#include "gamevars.hpp"
#include "entity.hpp"
#include "tile.hpp"

class tile;

class attackType {
	const std::string name;

	const int numAttacks;
	const int damage;
	const float hitRate;

	public:
		attackType() = delete;
		attackType(const std::string& name, const int numAttacks,
				const int damage, const float hitRate): name(name),
			numAttacks(numAttacks), damage(damage), hitRate(hitRate) {}

		std::string Name() const		{return name;}
		int NumAttacks() const			{return numAttacks;}
		int Damage() const				{return damage;}
		float HitRate() const			{return hitRate;}
};

class unitType {
	const int id;
	const std::string name;
	
	const int maxHealth;
	const std::vector<std::shared_ptr<attackType>> attacks;

	const int trainingTime;

	bool canRespec = true;

	public:
		unitType() = delete;
		unitType(const int id, const std::string& name, const int maxHealth,
			const std::vector<std::shared_ptr<attackType>>& attacks, 
			const int trainingTime):
			id(id), name(name), maxHealth(maxHealth), attacks(attacks),
			trainingTime(trainingTime) {}

		int ID()			const			{return id;}
		std::string Name()	const			{return name;}
		int MaxHealth()		const			{return maxHealth;}
		int TrainingTime()	const			{return trainingTime;}

		bool CanRespec()	const 			{return canRespec;}
		void SetCanRespec(const bool val)	{canRespec = val;}

		std::vector<std::shared_ptr<attackType>> Attacks() const	{return attacks;}
};

class person : public entity {
	std::string givenName;
	std::string surname;
	bool female;
	std::shared_ptr<unitType> spec;
	
	int maxHealth;
	int health;
	std::vector<std::string> inventory;

	char faction;
	std::shared_ptr<tile> location;

	static std::string GenerateGivenName();
	static std::string GenerateSurname();

	public:
		person() = delete;
		person(SDL_Renderer* ren, const std::shared_ptr<unitType> spec,
				const char faction);
		person(const person& other) = delete;
		person& operator=(const person other) = delete;

		void ChangeName(const std::string givenName, const std::string surname);
		void ChangeGender(const std::string gender);
		void ChangeSpec(const std::shared_ptr<unitType> spec);
		void ChangeMaxHealth(const int maxHealth);
		bool TakeDamage(const int damage); // false = survived
		void Die();
		bool Dead() const { return health <= 0; }
		void AddItem(const std::string item);

		void Render() const;

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
		std::shared_ptr<unitType>	Spec()	 	const;
		int							MaxHealth() const;
		int							Health()	const;
		std::vector<std::string> 	Inventory() const;

		void MoveToTile(std::shared_ptr<tile> newLoc);
		std::shared_ptr<tile> Location() const;
		int Row() const;
		int Colm() const;

		bool CanRespec() const			{return spec->CanRespec();}
		char Faction() const			{return faction;}

		std::vector<std::shared_ptr<attackType>> Attacks() const {return spec->Attacks();}
		void Attack(std::shared_ptr<person>& target) const;
		static void Fight(std::shared_ptr<person> attacker, 
						std::shared_ptr<person> target);
};

#endif
