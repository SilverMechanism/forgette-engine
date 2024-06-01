module;
export module unit;
import entity;
import sprite;
import movement;

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
		
		std::vector<ptr::watcher<Unit>> welded_units;
		
		virtual void render_update() override;
		virtual void game_update(float delta_time) override;
		
		void set_sprite(std::string new_sprite_name);
		std::string get_sprite_name();
		
		virtual void on_spawn() override;
		
		float radius = 12.0f;
		Movement movement;
		
		float health = 100.f;
		
		coordinates<float> map_location;
		coordinates<float> get_map_location() const;
		void set_map_location(coordinates<float> new_map_location);
		
	protected:
		ptr::keeper<Sprite> sprite;
		std::string sprite_name;
		
	private:
	};
}

void Unit::game_update(float delta_time)
{
	for (auto &unit : welded_units)
	{
		if (Unit* welded_unit = unit.get())
		{
			welded_unit->set_map_location(get_map_location());
		}
	}
}

void Unit::render_update()
{
	if (Sprite* sprite_ptr = sprite.get())
	{
		sprite_ptr->render_to_map(get_map_location(), true);
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

coordinates<float> Unit::get_map_location() const
{
	return map_location;
}

void Unit::set_map_location(coordinates<float> new_location)
{
	map_location = new_location;
}