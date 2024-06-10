module;
export module sprite_sheet;
import sprite;
import core;
import std;
import sprite;
import database;

export
{
	struct frame_data
	{
		coordinates<float> position;
		coordinates<float> size;
	};
	
	class SpriteSheet
	{
	public:
		SpriteSheet(std::string name, const ptr::keeper<Sprite> &sprite_keeper);
		
		ptr::watcher<Sprite> sprite;
		std::vector<frame_data> frames;
		
		void update_sprite_atlas(int frame_index);
	};
}

SpriteSheet::SpriteSheet(std::string name, const ptr::keeper<Sprite> &sprite_keeper)
{
	sprite = ptr::watcher<Sprite>(sprite_keeper);
	// std::cout << "Sprite sheet using Sprite " << sprite_keeper.get() << std::endl;
	database::regular_sheet sheet = database::get_sprite_sheet(name);
	coordinates<float> image_dimensions = sprite->get_image_dimensions();
	int sprites_x = static_cast<int>(image_dimensions.x)/sheet.x;
	
	for (int i = 0; i < sprites_x; i++)
	{
		frames.push_back({coordinates<float>(float(sheet.x*i), 0.0f), coordinates<float>(float(sheet.x), float(sheet.y))});
	}
	
	update_sprite_atlas(1);
}

void SpriteSheet::update_sprite_atlas(int frame_index)
{
	if (!frame_index)
	{
		frame_index++;
	}
	
	int true_index = std::abs(frame_index);
	float sign = float(true_index/frame_index);
	
	// std::cout << "Updating sprite " << sprite.get() << std::endl;
	
	frame_data fd = frames[true_index-1];
	sprite->atlas_size = coordinates<float>(fd.size.x * sign, fd.size.y);
	sprite->atlas_location = fd.position;
}