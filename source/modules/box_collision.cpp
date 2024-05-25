module;
export module box_collision;
import collision;
import core;

export
{
	class BoxCollision : public Collision
	{
	public:
		BoxCollision(coordinates<float> box_extent);
		
		// In essence the (x, y) of the top-right coordinate
		coordinates<float> extent = coordinates(50.f, 50.f);
	};
}

BoxCollision::BoxCollision(coordinates<float> box_extent)
{
	extent = box_extent;
}