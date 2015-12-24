#include <iostream>
#include <assert.h>
#include <functional>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Character
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
};

struct Spell
{
	explicit Spell(ESpellName InName, int InCost) : Name(InName), Cost(InCost) { }
	
	typedef function<void (Character&, Character&)> Action;
	Action		OnCast		= [] (Character& inPlayer, Character& inBoss) { };
	Action		OnTurnStart	= [] (Character& inPlayer, Character& inBoss) { };
	Action		OnEnd		= [] (Character& inPlayer, Character& inBoss) { };

	ESpellName	Name;
	int			Cost;
	int			LifeTime = 0;
};

static const Spell kMagicMissile = [] ()
{
	Spell spell(MagicMissile, 53);
	spell.OnCast = [] (Character& inPlayer, Character& inBoss) { inBoss.HP -= 4; };
	return spell;
} ();

static const Spell kDrain = [] ()
{
	Spell spell(Drain, 73);
	spell.OnCast = [] (Character& inPlayer, Character& inBoss) { inBoss.HP -= 2; inPlayer.HP += 2; };
	return spell;
} ();

static const Spell kPoison = [] ()
{
	Spell spell(Poison, 173);
	spell.OnTurnStart = [] (Character& inPlayer, Character& inBoss) { inBoss.HP -= 3; };
	spell.LifeTime = 6;
	return spell;
} ();

static const Spell kShield = [] ()
{
	Spell spell(Shield, 113);
	spell.OnCast	= [] (Character& inPlayer, Character& inBoss) { inPlayer.Def += 7; };
	spell.OnEnd		= [] (Character& inPlayer, Character& inBoss) { inPlayer.Def -= 7; };
	spell.LifeTime = 6;
	return spell;
} ();

static const Spell kRecharge = [] ()
{
	Spell spell(Recharge, 229);
	spell.OnTurnStart = [] (Character& inPlayer, Character& inBoss) { inPlayer.Mana += 101; };
	spell.LifeTime = 5;
	return spell;
} ();

static const vector<Spell> kSpells = 
{
	kMagicMissile,
	kDrain,
	kPoison,
	kShield,
	kRecharge
};

Character CreatePlayer()
{
	Character c;
	c.HP	= 50;
	c.Mana	= 500;
	c.Atk	= 0;
	c.Def	= 0;
	return c;
}

Character CreateBoss()
{
	Character c;
	c.HP	= 51;
	c.Mana	= 0;
	c.Atk	= 9;
	c.Def	= 0;
	return c;
}

void DoSpellEffects(Character& inPlayer, Character& inBoss, vector<Spell>& inEffects)
{
	for (int i = (int) inEffects.size() - 1; i >= 0; --i)
	{
		auto& effect = inEffects[i];
		effect.OnTurnStart(inPlayer, inBoss);
		effect.LifeTime--;

		if (effect.LifeTime <= 0)
		{
			effect.OnEnd(inPlayer, inBoss);
			inEffects.erase(inEffects.begin() + i);
		}
	}
}

void CastSpell(Character& inPlayer, Character& inBoss, vector<Spell>& inEffects, const Spell& inSpell)
{
	inPlayer.Mana -= inSpell.Cost;
	inSpell.OnCast(inPlayer, inBoss);
	if (inSpell.LifeTime > 0)
	{
		inEffects.push_back(inSpell);
	}
}

void DoBossAttack(Character& inPlayer, Character& inBoss)
{
	inPlayer.HP -= max(inBoss.Atk - inPlayer.Def, 0);
}

enum ERoundResult
{
	NoResult,
	Failed,
	Success
};

ERoundResult GetResult(Character& inPlayer, Character& inBoss)
{
	if (inPlayer.Mana <= 0) return Failed;
	if (inPlayer.HP <= 0)	return Failed;
	if (inBoss.HP <= 0)		return Success;
	return NoResult;
}

bool IsSpellActive(ESpellName inSpellName, const vector<Spell>& inEffects)
{
	for (auto& effect : inEffects)
	{
		if (effect.Name == inSpellName)
		{
			return true;
		}
	}

	return false;
}

ERoundResult DoRound(Character& inPlayer, Character& inBoss, vector<Spell>& inEffects, const Spell& inSpell, bool inDecayHealth)
{
	auto Result		= [&] { return GetResult(inPlayer, inBoss); };
	auto IsFinished = [&] { return Result() != NoResult; };

	DoSpellEffects(inPlayer, inBoss, inEffects);
	if (inDecayHealth) inPlayer.HP--;
	if (IsFinished()) return Result();

	if (IsSpellActive(inSpell.Name, inEffects)) { return Failed; }

	CastSpell(inPlayer, inBoss, inEffects, inSpell);
	if (IsFinished()) return Result();

	DoSpellEffects(inPlayer, inBoss, inEffects);
	if (IsFinished()) return Result();

	DoBossAttack(inPlayer, inBoss);
	return Result();
}

struct SpellStack
{
	vector<ESpellName>	Spells;
	int					Cost = 0;
};

void DoNextStep(Character inPlayer, Character inBoss, vector<Spell> inEffects, Spell inSpell, bool inDecayHealth, SpellStack inCurrentSpellStack, SpellStack& ioBestSpellStack)
{
	inCurrentSpellStack.Spells.push_back(inSpell.Name);
	inCurrentSpellStack.Cost += inSpell.Cost;

	ERoundResult result = DoRound(inPlayer, inBoss, inEffects, inSpell, inDecayHealth);

	if (inCurrentSpellStack.Cost > ioBestSpellStack.Cost)
	{
		return;
	}

	switch (result)
	{
		case Success:
			ioBestSpellStack = inCurrentSpellStack;
		case Failed:
			break;

		case NoResult:
		{	
			for (auto& spell : kSpells)
			{
				DoNextStep(inPlayer, inBoss, inEffects, spell, inDecayHealth, inCurrentSpellStack, ioBestSpellStack);
			}
			break;
		}
	}
}

int FindCheapestCostToWin(bool inDecayHealth)
{
	SpellStack best;
	best.Cost = INT_MAX;

	for (auto& spell : kSpells)
	{
		DoNextStep(CreatePlayer(), CreateBoss(), vector<Spell>(), spell, inDecayHealth, SpellStack(), best);
	}

	return best.Cost;
}

int main()
{
	int cost1 = FindCheapestCostToWin(false);
	int cost2 = FindCheapestCostToWin(true);

	cout << "Cost test 1 = " << cost1 << endl;
	cout << "Cost test 2 = " << cost2 << endl;

	return 0;
}
