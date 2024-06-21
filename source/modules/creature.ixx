module;
export module creature;

import unit;

class Device;

export
{
	class Creature : public Unit
	{
	public:
		Creature();
		
		ptr::watcher<Device> active_device;
	};
}