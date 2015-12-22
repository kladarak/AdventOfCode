#include <iostream>
#include <assert.h>

#include "File.h"
#include "StringHelper.h"

using namespace std;

struct Stats
{
	int HP;
	int Mana;
	int Atk;
	int Def;
};

enum ESpellName
{
	MagicMissile,
	Drain,
	Shield,
	Poison,
	Recharge,

	Count,
};

struct Spell
{
	ESpellName	Name;
	Stats		DeltaStats;
	int			ManaCost;
	int			Duration;
};

static const vector<Spell> kSpells =
{
	{ MagicMissile, { 0, 0,		4, 0 }, 53,		0 },
	{ Drain,		{ 2, 0,		2, 0 }, 73,		0 },
	{ Shield,		{ 0, 0,		0, 7 }, 113,	6 },
	{ Poison,		{ 0, 0,		3, 0 }, 173,	6 },
	{ Recharge,		{ 0, 101,	0, 0 }, 229,	5 },
};

struct Character
{
	Stats CurrentStats;
};

Character CreatePlayer()
{
	Character c;
	c.CurrentStats.HP	= 50;
	c.CurrentStats.Mana = 500;
	c.CurrentStats.Atk	= 0;
	c.CurrentStats.Def	= 0;
	return c;
}

Character CreateBoss()
{
	Character c;
	c.CurrentStats.HP	= 51;
	c.CurrentStats.Mana = 0;
	c.CurrentStats.Atk	= 9;
	c.CurrentStats.Def	= 0;
	return c;
}

int main()
{
	Character			player	= CreatePlayer();
	Character			boss	= CreateBoss();
	vector<ESpellName>	activeSpells;



	return 0;
}
