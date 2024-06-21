export module core:entity_class;

import std;

export
{
	enum class EntityClass : std::uint16_t
	{
		Unit,
		DebugUnit,
		Villager,
		Creature,
		FloatingText,
		MagicText,
		Hero,
		HeroArcher
	};
}
