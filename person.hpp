#ifndef PERSON_HPP
#define PERSON_HPP

#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

class person {
	std::string givenName;
	std::string surname;
	bool female;
	std::string spec;
	
	int maxHealth;
	int health;
	std::vector<std::string> inventory;

	static std::string GenerateGivenName();
	static std::string GenerateSurname();

	public:
		person();

		void ChangeName(const std::string givenName, const std::string surname);
		void ChangeGender(const std::string gender);
		void ChangeSpec(const std::string spec);
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
		std::string 				Spec()	 	const;
		int							MaxHealth() const;
		int							Health()	const;
		std::vector<std::string> 	Inventory() const;
};

#endif
