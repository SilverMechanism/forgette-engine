module;
#include <cassert>
export module sprite_entity;
import core;
import entity;
import sprite;

export
{
	class SpriteEntity : public Entity
	{
	public:
		SpriteEntity();
		
		virtual void game_update(float delta_time) override;
		virtual void render_update() override;
		virtual bool initialize() override;
		bool initialize(std::wstring new_image_path);
		
		bool reload_sprite(bool strict);
		bool reload_sprite(bool strict, std::wstring new_image_path);
	protected:
		std::wstring image_path;
	private:
		std::unique_ptr<Sprite> sprite = nullptr;
		
	};
}

SpriteEntity::SpriteEntity()
{
	should_render_update = true;
	display_name = "Sprite Entity";
}

bool SpriteEntity::initialize()
{
	Entity::initialize();
	return reload_sprite(true);
}

void SpriteEntity::game_update(float delta_time)
{
	
}

void SpriteEntity::render_update()
{
	if (sprite)
	{
		sprite->render_to_map(get_map_location(), true);
	}
}

bool SpriteEntity::reload_sprite(bool strict)
{
	if (!image_path.empty())
	{
		sprite  = std::make_unique<Sprite>(image_path);
		
		return sprite.get();
	}
	else if (strict)
	{
		assert("Image path not set when reloading sprite");
	}
	
	return false;
}

bool SpriteEntity::reload_sprite(bool strict, std::wstring new_image_path)
{
	image_path = new_image_path;
	
	if (!image_path.empty())
	{
		sprite  = std::make_unique<Sprite>(image_path);
		
		return sprite.get();
	}
	else if (strict)
	{
		assert("Image path not set when reloading sprite");
	}
	
	return false;
}