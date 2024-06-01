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
	};
}

bool Entity::initialize()
{
	return true;
}

void Entity::render_update()
{

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