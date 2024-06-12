module;
export module gm_survival;

import game_mode;

export
{
	class GM_Survival : public GameMode
	{
	public:
		virtual void start() override;
	};
}