module;
export module state;
import unit;

export
{
	class State
	{
	public:
		virtual void enter(Unit* unit) = 0;
		virtual void update(Unit* unit, float delta_time) = 0;
		virtual void exit(Unit* unit) = 0;
	};
}