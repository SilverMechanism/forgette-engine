module;
module special_effect;

SpecialEffect::SpecialEffect()
{
	should_render_update = true;
}

void SpecialEffect::render_update(RenderGroup rg)
{
	if (effects.find(rg) != effects.end())
	{
		for (auto effect_functions : effects[rg])
		{
			effect_functions();
		}
	}
}