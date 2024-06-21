export module unit;

import entity;
import core;
import game_map;

export
{
	// Forward declarations
	class Sprite;

    class Unit : public Entity
    {
    public:
        SectorInfo current_sector_info;
        Unit();
        
        std::vector<ptr::watcher<Unit>> welded_units;

        virtual void render_update(RenderGroup rg) override;
        virtual void game_update(float delta_time) override;

        void set_sprite(std::string new_sprite_name);
        std::string get_sprite_name();
        ptr::keeper<Sprite> sprite;

        virtual void on_spawn() override;

        float radius = 8.0f;

        coordinates<float> map_location;
        coordinates<float> get_map_location() const;
        void set_map_location(coordinates<float> new_map_location, bool no_check=false);

        virtual void on_death()
        {
        	
        }
        
    protected:
        std::string sprite_name;

    private:
    	coordinates<float> last_location;
    };
}
