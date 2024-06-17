module;
export module special_effect;

import entity;

export
{
	class SpecialEffect : public Entity
	{
	public:
		SpecialEffect();
		
		ColorParams effect_color;
		std::unordered_map<RenderGroup, std::vector<std::function<void()>>> effects;
		
		virtual void render_update(RenderGroup rg) override;
	};
}