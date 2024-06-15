module;
export module gm_survival;

import game_mode;

export
{
	class GM_Survival : public GameMode
	{
	public:
		void spawn_wave();
		virtual void start() override;
	};
}