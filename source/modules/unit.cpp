module;
export module unit;
import entity;
import sprite;
import movement;
import core;
import helpers;

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
		
		bool collision_enabled = true;
		
		std::vector<ptr::watcher<Unit>> welded_units;
		
		virtual void render_update() override;
		virtual void game_update(float delta_time) override;
		
		void set_sprite(std::string new_sprite_name);
		std::string get_sprite_name();
		ptr::keeper<Sprite> sprite;
		
		virtual void on_spawn() override;
		
		float radius = 8.0f;
		Movement movement;
		
		float health = 100.f;
		
		void damage(float amount)
		{
			health -= amount;
			Helpers::create_floating_text(
				std::to_string(static_cast<int>(amount)), 
				16.0f, 
				get_map_location(), 
				1.0f, 
				coordinates<float>(0.0f, 100.0f).view_isometric(), 
				RenderGroup::Debug, 
				16.0f);
			
			if (health <= 0.0f)
			{
				pending_deletion = true;
			}
		}
		
		coordinates<float> map_location;
		coordinates<float> get_map_location() const;
		void set_map_location(coordinates<float> new_map_location);
		
		virtual void on_collision(Unit* other_unit) {};
		
		std::vector<std::int64_t> ignored_entities;
	protected:
		
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
	
	// std::cout << "Entity " << id << " using location for rendering: " << std::string(get_map_location()) << std::endl;
}

void Unit::set_sprite(std::string new_sprite_name)
{	
	/* if (!sprite.get())
	{
		Sprite* new_sprite = new Sprite(new_sprite_name);
		sprite = ptr::keeper<Sprite>(new_sprite);
	}
	else
	{
		sprite.get()->load(new_sprite_name);
	}
	
	sprite_name = new_sprite_name; */
}

std::string Unit::get_sprite_name()
{
	return sprite_name;
}

void Unit::on_spawn()
{
	// set_sprite(sprite_name);
}

coordinates<float> Unit::get_map_location() const
{
	return map_location;
}

void Unit::set_map_location(coordinates<float> new_location)
{
	map_location = new_location;
}