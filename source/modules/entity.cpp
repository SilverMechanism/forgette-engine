module;
export module entity;
import core;
import std;
import directx;
import timers;
import element;

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
		virtual void render_update(RenderGroup rg);
		virtual bool initialize();
		std::string get_display_name();
		
		virtual void on_spawn();
		
		template <typename T>
		T* add_element()
		{
			if (T* existing_element = get_element<T>())
			{
				return existing_element;
			}
			
			ptr::keeper<Element> element_keeper = ptr::keeper<Element>(new T());
			elements[typeid(T)] = std::move(element_keeper);
			if (T* new_element = get_element<T>())
			{
				new_element->set_owner(this);
				return new_element;
			}
			else
			{
				return nullptr;
			}
		}
		
		template<typename T>
		T* get_element()
		{
			auto iterator = elements.find(typeid(T));
			if (iterator != elements.end())
			{
				return static_cast<T*>(iterator->second.get());
			}
			return nullptr;
		}

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
		std::unordered_map<std::type_index, ptr::keeper<Element>> elements;
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

void Entity::render_update(RenderGroup rg)
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