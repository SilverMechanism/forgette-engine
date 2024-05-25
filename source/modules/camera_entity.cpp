module;
export module camera_entity;
import core;
import input;
import entity;
import directx;
import unit;

export
{
	class CameraEntity : public Entity
	{
	public:
		CameraEntity();
		
		void ZoomIn();
		void ZoomOut();
		
		virtual void game_update(float delta_time) override;
		
		ptr::watcher<Unit> lock_on_target;
	};
}

CameraEntity::CameraEntity()
{
	display_name = "Camera Entity";
	should_game_update = true;
}

void CameraEntity::ZoomIn()
{
	float current_zoom = ForgetteDirectX::get_zoom_level();
	ForgetteDirectX::set_zoom_level(current_zoom + 0.01f);
}

void CameraEntity::ZoomOut()
{
	float current_zoom = ForgetteDirectX::get_zoom_level();
	ForgetteDirectX::set_zoom_level(current_zoom - 0.01f);
}

void CameraEntity::game_update(float delta_time)
{
	if (lock_on_target.get())
	{
		set_map_location(lock_on_target->get_map_location());
	}
}