module;
export module unit;
import entity;

export
{
	class Unit : public Entity
	{
	public:
		Unit()
		{
			display_name = "Unit";
		}
		
		coordinates<float> get_map_location();
		void set_map_location(coordinates<float> new_location);
		
	protected:
		std::uint64_t id;
		
	private:
		coordinates<float> map_location {0.f, 0.f};
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