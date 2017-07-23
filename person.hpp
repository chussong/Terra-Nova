#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <random>
#include <functional>
#include "gamevars.hpp"
#include "entity.hpp"
#include "path.hpp"

struct moveCostTable {
	int base = 1;
	int aquatic = -1;
	int wooded = 1;
	int hilly = 1;
	int cold = 1;
};

class person;
struct Order {
	std::string name;
	std::function<void()> func;

	Order(const std::string& name, std::function<void()> func): 
		name(name), func(func){}
	//Order(const char* name, void (*func)()): name(std::string(name)), func(func){}
};

class attackType {
	const std::string name;

	const float accuracy;
	const int attackRate;
	const int damage;

	public:
		attackType() = delete;
		attackType(const std::string& name, const float accuracy,
				const int attackRate, const int damage): name(name),
			accuracy(accuracy), attackRate(attackRate), damage(damage) {}

		std::string Name() const		{return name;}
		float Accuracy() const			{return accuracy;}
		int AttackRate() const			{return attackRate;}
		int Damage() const				{return damage;}
		std::string DamageType() const	{return "ballistic";}
};

class unitType {
	private:

	const std::string name;
	
	const int maxHealth;
	const int moveSpeed;
	const moveCostTable moveCosts;
	const std::vector<std::shared_ptr<attackType>> attacks;

	const int trainingTime;

	bool canRespec = true;

	public:
	// ultimately we want to construct this from a line of text as std::string
		unitType() = delete;
		unitType(const std::string& name, const int maxHealth,
			const int moveSpeed,
			const std::vector<std::shared_ptr<attackType>>& attacks, 
			const int trainingTime):
			name(name), maxHealth(maxHealth), moveSpeed(moveSpeed), 
			attacks(attacks), trainingTime(trainingTime) {}

		std::string Name()	const			{return name;}
		int MaxHealth()		const			{return maxHealth;}
		int MoveSpeed()		const			{return moveSpeed;}
		moveCostTable MoveCosts() const		{return moveCosts;}
		int TrainingTime()	const			{return trainingTime;}

		bool CanRespec()	const 			{return canRespec;}
		void SetCanRespec(const bool val)	{canRespec = val;}

		std::vector<std::shared_ptr<attackType>> Attacks() const {return attacks;}
		std::shared_ptr<attackType> Attack(const int num) const 
			{if(num >= (int)attacks.size()) return nullptr; return attacks[num];}
};

class person : public entity {
	std::shared_ptr<gfxObject> healthBackground;
	std::shared_ptr<gfxObject> healthBar;

	std::string givenName;
	std::string surname;
	bool female;
	std::shared_ptr<unitType> spec;
	
	int health;
	int movesLeft;
	std::vector<std::string> inventory;

	char faction;
	std::array<unsigned int, 2> location;
	std::unique_ptr<path> myPath;
	order_t orders = ORDER_PATROL;
	std::shared_ptr<gfxObject> orderIcon;
	std::vector<Order> availableOrders;

	static std::string GenerateGivenName();
	static std::string GenerateSurname();

	public:
		person() = delete;
		person(SDL_Renderer* ren,
				const std::shared_ptr<unitType> spec, const char faction);
		person(const person& other) = delete;
		person& operator=(const person other) = delete;

		void ChangeName(const std::string givenName, const std::string surname);
		void ChangeName(const std::string fullName);
		void ChangeGender(const std::string gender);
		void ChangeSpec(const std::shared_ptr<unitType> spec);
		bool TakeDamage(const int damage); // false = survived
		void Die();
		bool Dead() const { return health <= 0; }
		bool IsUnit() const { return true; }

		void MoveSpriteToTile(const int X, const int Y, const int W, const int H);
		void Render() const;
		void ProcessTurn();

		std::string					Name()		const;
		std::string 				GivenName() const;
		std::string 				Surname() 	const;
		std::string					Species()	const;
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
		void AddItem(const std::string item);
		int							MoveSpeed()	const;
		int							MovesLeft()	const;

		void SetLocation(const unsigned int row, const unsigned int colm, 
				const bool usesMove);
		std::array<unsigned int, 2> Location() const;
		int Row() const;
		int Colm() const;
		path* Path() const { return myPath.get(); }
		std::array<unsigned int, 2> NextStep();
		bool AdvancePath();
		moveCostTable MoveCosts() const {return spec->MoveCosts(); }
		order_t Orders() const { return orders; }
		void SetOrders(const order_t newOrders);
		void OrderMove(std::unique_ptr<path> newPath);
		void OrderPatrol();
		void OrderHarvest();
		const std::vector<Order>& AvailableOrders() const { return availableOrders; }

		bool CanRespec() const			{return spec->CanRespec();}
		char Faction() const			{return faction;}

		std::vector<std::shared_ptr<attackType>> Attacks() const;
		int Damage(const unsigned int num = 0) const;
		float Accuracy(const unsigned int num = 0) const;
		int AttackRate(const unsigned int num = 0) const;
		std::string DamageType(const unsigned int num = 0) const;

		bool CanAttack() const { return Attacks().size() > 0; }
		bool Attack(person* target) const;
		static void Fight(person* attacker, person* target);
};

#endif
