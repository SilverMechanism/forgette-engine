module;
export module sector;
import std;
import entity;

export
{
	class Sector
	{
	public:
		Sector();
		
		std::vector<std::weak_ptr<Entity>> entities;
		
		void cleanup_expired_entities();
	};
}

Sector::Sector()
{

}
	
void Sector::cleanup_expired_entities()
{
	int num_expired = 0;
	int vector_size = static_cast<int>(entities.size());
	for (int i = 0; i < vector_size; i++)
	{
		if (entities[i].expired())
		{
			std::swap(entities[i], entities[vector_size - num_expired - 1]);
			num_expired++;
		}
	}
	entities.erase(entities.end() - num_expired, entities.end());
}