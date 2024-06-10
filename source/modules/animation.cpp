module;
export module animation;
import core;
import std;
import sprite_sheet;

export
{
	class Animation
	{
	public:
		// Animation(const &ptr::keeper<SpriteSheet> _sprite_sheet, std::vector<int> frame_order, float frame_duration);
		
		void run(float delta_time);
		ptr::watcher<SpriteSheet> sprite_sheet;
		
		std::string name;
	};
}