#ifndef UNIT_HPP
#define UNIT_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <random>
#include <functional>
#include "gamevars.hpp"
#include "file.hpp"
#include "gfxobject.hpp"
#include "path.hpp"

namespace TerraNova {

struct MoveCostTable {
	int base = 1;
	int aquatic = -1;
	int wooded = 1;
	int hilly = 1;
	int cold = 1;
};

// should also have support for orders with targets (different struct maybe?)
struct Order {
	std::string name;
	std::function<void()> func;

	Order(const std::string& name, std::function<void()> func): 
		name(name), func(func){}
	//Order(const char* name, void (*func)()): name(std::string(name)), func(func){}
};

class AttackType {
	const std::string name;

	const float accuracy;
	const int attackRate;
	const int damage;

	public:
		AttackType() = delete;
		AttackType(const std::string& name, const float accuracy,
				const int attackRate, const int damage): name(name),
			accuracy(accuracy), attackRate(attackRate), damage(damage) {}

		std::string Name() const		{return name;}
		float Accuracy() const			{return accuracy;}
		int AttackRate() const			{return attackRate;}
		int Damage() const				{return damage;}
		std::string DamageType() const	{return "ballistic";}
};

class UnitType {
	public:
	// ultimately we want to construct this from a line of text as std::string
		UnitType() = delete;
		UnitType(const std::string& name, const int maxHealth,
			const int moveSpeed,
			const std::vector<std::shared_ptr<AttackType>>& attacks, 
			const int trainingTime):
			name(name), maxHealth(maxHealth), moveSpeed(moveSpeed), 
			attacks(attacks), trainingTime(trainingTime) {}

		const std::string& Name() const			{return name;}
		std::string PathName() const 		{return boost::to_lower_copy(name);}
		int MaxHealth()		const			{return maxHealth;}
		int MoveSpeed()		const			{return moveSpeed;}
		MoveCostTable MoveCosts() const		{return moveCosts;}
		int TrainingTime()	const			{return trainingTime;}

		bool CanRespec()	const 			{return canRespec;}
		void SetCanRespec(const bool val)	{canRespec = val;}

		std::vector<std::shared_ptr<AttackType>> Attacks() const {return attacks;}
		std::shared_ptr<AttackType> Attack(const int num) const 
			{if(num >= (int)attacks.size()) return nullptr; return attacks[num];}

	private:
		const std::string name;
		
		const int maxHealth;
		const int moveSpeed;
		const MoveCostTable moveCosts;
		const std::vector<std::shared_ptr<AttackType>> attacks;

		const int trainingTime;

		bool canRespec = true;
};

class Unit : public GFXObject {
	std::shared_ptr<Sprite> healthBackground;
	std::shared_ptr<Sprite> healthBar;

	std::string givenName;
	std::string surname;
	bool female;
	std::shared_ptr<UnitType> spec;
	bool unique = false;
	
	int health;
	int movesLeft;
	std::vector<std::string> inventory;

	char faction;
	std::array<unsigned int, 2> location;
	std::unique_ptr<Path> myPath;
	order_t orders = ORDER_PATROL;
	std::shared_ptr<Sprite> orderIcon;
	//std::vector<Order> availableOrders;

	void Die();

	static std::string GenerateGivenName();
	static std::string GenerateSurname();

	public:
		Unit() = delete;
		Unit(SDL_Renderer* ren,
				const std::shared_ptr<UnitType> spec, const char faction);
		Unit(const Unit& other) = delete;
		Unit& operator=(const Unit other) = delete;

		void ChangeName(const std::string givenName, const std::string surname);
		void ChangeName(const std::string fullName);
		void ChangeGender(const std::string gender);
		void ChangeSpec(const std::shared_ptr<UnitType> spec);
		bool TakeDamage(const int damage); // false = survived
		bool Dead() const { return health <= 0; }
		bool IsUnit() const { return true; }
		void BeConsumed();

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
		std::shared_ptr<UnitType>	Spec()	 	const;
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
		Path* CurrentPath() const { return myPath.get(); }
		std::array<unsigned int, 2> NextStep();
		bool AdvancePath();
		MoveCostTable MoveCosts() const {return spec->MoveCosts(); }
		order_t Orders() const { return orders; }
		void SetOrders(const order_t newOrders);
		void OrderMove(std::unique_ptr<Path> newPath);
		void OrderPatrol();
		void OrderHarvest();
		//std::shared_ptr<Colony> OrderFoundColony();
		// below function is not const because the buttons can make *this act
		std::vector<Order> AvailableOrders();

		void MarkUnique()				{unique = true;}
		bool Unique() const				{return unique;}
		bool CanRespec() const			{return spec->CanRespec();}
		char Faction() const			{return faction;}

		std::vector<std::shared_ptr<AttackType>> Attacks() const;
		int Damage(const unsigned int num = 0) const;
		float Accuracy(const unsigned int num = 0) const;
		int AttackRate(const unsigned int num = 0) const;
		std::string DamageType(const unsigned int num = 0) const;

		bool CanAttack() const { return Attacks().size() > 0; }
		bool Attack(Unit* target) const;
		static void Fight(Unit* attacker, Unit* target);
};

} // namespace TerraNova
#endif
