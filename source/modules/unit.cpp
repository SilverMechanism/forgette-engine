module;
export module unit;
import entity;
import sprite;

export
{
	class Unit : public Entity
	{
	public:
		Unit()
		{
			display_name = "Unit";
			should_render_update = true;
		}
		
		coordinates<float> get_map_location();
		void set_map_location(coordinates<float> new_location);
		
		std::vector<ptr::watcher<Entity>> welded_entities;
		
		virtual void render_update() override;
		virtual void game_update(float delta_time) override;
		
		void set_sprite(std::string new_sprite_name);
		std::string get_sprite_name();
		
		virtual void on_spawn() override;
		
	protected:
		std::uint64_t id;
		coordinates<float> map_location {0.f, 0.f};
		ptr::keeper<Sprite> sprite;
		std::string sprite_name;
		
	private:
	};
}

coordinates<float> Unit::get_map_location()
{
	return map_location;
}

void Unit::set_map_location(coordinates<float> new_location)
{
	map_location = new_location;
}

void Unit::game_update(float delta_time)
{
	for (auto &entity : welded_entities)
	{
		if (Entity* welded_entity = entity.get())
		{
			welded_entity->set_map_location(get_map_location());
		}
	}
}

void Unit::render_update()
{
	if (Sprite* sprite_ptr = sprite.get())
	{
		sprite_ptr->render_to_map(map_location, true);
	}
}

void Unit::set_sprite(std::string new_sprite_name)
{	
	if (!sprite.get())
	{
		Sprite* new_sprite = new Sprite(new_sprite_name);
		sprite = ptr::keeper<Sprite>(new_sprite);
	}
	else
	{
		sprite.get()->load(new_sprite_name);
	}
	
	sprite_name = new_sprite_name;
}

std::string Unit::get_sprite_name()
{
	return sprite_name;
}

void Unit::on_spawn()
{
	set_sprite(sprite_name);
}