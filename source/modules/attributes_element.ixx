module;
#include "defines.h"
export module attributes_element;

import std;
import element;

export
{
	class AttributesElement : public Element
	{
		float experience {0.0f};
		uint16 level {0};
		
		int16 strength {0};
		int16 agility {0};
		int16 fortitude {0};
		
		int16 clarity {0};
		int16 expertise {0};
		int16 arcane {0};
		
		int16 wisdom {0};
		int16 intuition {0};
		int16 resilience {0};
		
		constexpr std::int32_t get_body()
		{
			return (strength + agility + fortitude);
		}
		constexpr std::int32_t get_mind()
		{
			return (clarity + expertise + arcane);
		}
		constexpr std::int32_t get_spirit()
		{
			return (wisdom + intuition + resilience);
		}
	};
}