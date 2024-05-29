module;
export module entity;
import core;
import std;
import directx;
import timers;

export
{
	class Entity
	{
	public:
		Entity() = default;
		void set_map_location(coordinates<float> new_location);
		coordinates<float> get_map_location();

		virtual void game_update(float delta_time);
		virtual void render_update();
		virtual bool initialize();
		std::string get_display_name();
		
		virtual void on_spawn();

		bool should_game_update = false;
		bool should_render_update = false;
		
		coordinates<float> size {1.0f, 1.0f};
		
		std::int64_t id = -1;
	protected:
		std::string display_name = "Entity";
		
	private:
		coordinates<float> map_location {0.f, 0.f};
	};
}

bool Entity::initialize()
{
	return true;
}

void Entity::render_update()
{

}

void Entity::set_map_location(coordinates<float> new_location)
{
	map_location = new_location;
}

coordinates<float> Entity::get_map_location()
{
	return map_location;
}

void Entity::game_update(float delta_time)
{

}

std::string Entity::get_display_name()
{
	return display_name;
}

void Entity::on_spawn()
{
	
}