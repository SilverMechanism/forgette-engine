module;
export module entity;
import core;
import std;
import directx;
import timers;

export
{	
	// Performance "enhancer"...
	// Very fast way to check if something is dervied from Unit with no cast
	enum class EntityType : std::uint8_t
	{
		Phantom,
		UI,
		Unit
	};
	
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
		
		std::int64_t id = -1;
		
		bool pending_deletion = false;
		
		RenderGroup render_group = RenderGroup::Game;
		
		// Used for rendering purposes only
		float z = 0.0f;
	protected:
		// I would like to optimize this somehow
		// Maybe make a constant base name
		// and an empty string for a dynamic name
		// Dynamic names should be quite unusual (i think)
		std::string display_name = "Entity";
		EntityType type = EntityType::Phantom;
		
	private:
		
	public:
		EntityType get_type()
		{
			return type;
		}
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