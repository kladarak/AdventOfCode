#include <iostream>
#include <assert.h>
#include <queue>

#include "File.h"
#include "StringHelper.h"

using namespace std;

struct Item
{
	string Name = "Empty";
	int Cost = 0;
	int Atk = 0;
	int Def = 0;
};

struct Shop
{
	vector<Item> Weapons;
	vector<Item> Armour;
	vector<Item> Rings;
};

struct Character
{
	int HP;
	int Cost;
	int Atk;
	int Def;
};

Shop ParseShop(const vector<string>& inLines)
{
	Shop out;
	vector<Item>* items = nullptr;

	for (auto& line : inLines)
	{
		if (line.find("Weapons") == 0) { items = &out.Weapons; }
		else if (line.find("Armor") == 0) { items = &out.Armour; }
		else if (line.find("Rings") == 0) { items = &out.Rings; }
		else if (line != "\n")
		{
			vector<string> pieces = StringHelper::Delimit(line, " \n\t");

			Item item;
			item.Def = stoi(pieces[pieces.size() - 1]);
			item.Atk = stoi(pieces[pieces.size() - 2]);
			item.Cost = stoi(pieces[pieces.size() - 3]);
			item.Name = pieces.front();
			items->push_back(item);
		}
	}

	return out;
}

Character ParseBoss(const vector<string>& inLines)
{
	Character boss;
	boss.HP = stoi(StringHelper::Delimit(inLines[0], " \n").back());
	boss.Atk = stoi(StringHelper::Delimit(inLines[1], " \n").back());
	boss.Def = stoi(StringHelper::Delimit(inLines[2], " \n").back());
	return boss;
}

Character CreatePlayer(const Item& inWeapon, const Item& inArmour, const Item& inRing1, const Item& inRing2)
{
	Character player;
	player.HP = 100;
	player.Cost = inWeapon.Cost + inArmour.Cost + inRing1.Cost + inRing2.Cost;
	player.Atk = inWeapon.Atk + inArmour.Atk + inRing1.Atk + inRing2.Atk;
	player.Def = inWeapon.Def + inArmour.Def + inRing1.Def + inRing2.Def;
	return player;
}

struct PrioritiseLowestPrice
{
	bool operator()(const Character& inLHS, const Character& inRHS)
	{
		return inLHS.Cost > inRHS.Cost;
	}
};

struct PrioritiseHighestPrice
{
	bool operator()(const Character& inLHS, const Character& inRHS)
	{
		return inLHS.Cost < inRHS.Cost;
	}
};

typedef priority_queue<Character, vector<Character>, PrioritiseLowestPrice> CheapestPlayerQueue;
typedef priority_queue<Character, vector<Character>, PrioritiseHighestPrice> DearestPlayerQueue;

template<typename TQueue>
void PopulatePlayers(const Shop& inShop, TQueue& inPlayerQueue)
{
	for (auto& weapon : inShop.Weapons)
	{
		for (auto& armour : inShop.Armour)
		{
			for (size_t ring1 = 0; ring1 < inShop.Rings.size(); ++ring1)
			{
				for (size_t ring2 = (ring1 + 1); ring2 < inShop.Rings.size(); ++ring2)
				{
					inPlayerQueue.push(CreatePlayer(weapon, armour, inShop.Rings[ring1], inShop.Rings[ring2]));
				}
			}
		}
	}
}

bool CanBeatBoss(const Character& inPlayer, const Character& inBoss)
{
	Character player = inPlayer;
	Character boss = inBoss;

	auto attack = [] (const Character& inAttacker, Character& inDefender)
	{
		int power = inAttacker.Atk - inDefender.Def;
		power = max(power, 1);
		inDefender.HP -= power;
		return inDefender.HP <= 0;
	};

	while (true)
	{
		if (attack(player, boss)) { return true; }
		if (attack(boss, player)) { return false; }
	}
}

int CheapestPlayerCostThatCanWin(const Shop& inShop, const Character& inBoss)
{
	CheapestPlayerQueue players;
	PopulatePlayers(inShop, players);

	while (true)
	{
		Character player = players.top();
		players.pop();

		if (CanBeatBoss(player, inBoss))
		{
			return player.Cost;
		}
	}

	return 0;
}

int DearestPlayerCostThatStillLoses(const Shop& inShop, const Character& inBoss)
{
	DearestPlayerQueue players;
	PopulatePlayers(inShop, players);

	while (true)
	{
		Character player = players.top();
		players.pop();

		if (!CanBeatBoss(player, inBoss))
		{
			return player.Cost;
		}
	}

	return 0;
}

int main()
{
	Shop shop = ParseShop(FileHelper::GetLinesInFile("shop.txt"));
	Character boss = ParseBoss(FileHelper::GetLinesInFile("boss.txt"));

	// Add empty items to deal with "unequipped" variations.
	shop.Armour.push_back(Item());	// No armour
	shop.Rings.push_back(Item());	// No ring
	shop.Rings.push_back(Item());	// per hand

	int cheapestPlayerCostThatCanWin = CheapestPlayerCostThatCanWin(shop, boss);
	int dearestPlayerCostThatStillLoses = DearestPlayerCostThatStillLoses(shop, boss);

	cout << "Cheapest player cost that can win = " << cheapestPlayerCostThatCanWin << endl;
	cout << "Dearest player cost that still loses = " << dearestPlayerCostThatStillLoses << endl;

	return 0;
}
