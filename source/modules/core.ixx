module;
#include "defines.h"
export module core;

export import :core_math;
export import :coordinates;
export import :smart_pointers;
export import :entity_class;

import std;

#ifdef WIN64
    import <windows.h>;
#endif

static std::wstring exe_path;

export 
{
	inline const float gravity {333.0f};
	namespace DebugInfo
	{
		bool sector_highlight = false;
	}
	
    std::wstring get_exe_path();
    std::wstring get_exe_dir();
    void set_exe_path(std::wstring new_path);

    enum class Command : uint8
    {
        Fallback,
        Primary,
        StopPrimary,
        Secondary,
        StopSecondary,
        Tertiary,
        StopTertiary,
        Pulse
    };

    struct ColorParams
    {
        ColorParams(float _red, float _green, float _blue, float _alpha)
            : red(_red), green(_green), blue(_blue), alpha(_alpha) {}

        ColorParams()
            : red(0.8f), green(0.8f), blue(0.8f), alpha(1.0f) {}

        float red;
        float green;
        float blue;
        float alpha;
    };

    enum class RenderGroup : std::uint8_t
    {
        PreGame,
        Game,
        PostGame,
        UI,
        Debug
    };

    enum class CollisionGroup : std::uint8_t
    {
        Unit,
        Projectile,
        Prop
    };

    struct Target
    {
        Target(class Unit* target_unit, coordinates<float> target_location)
            : unit(target_unit), location(target_location) {}

        class Unit* unit;
        coordinates<float> location;
    };

    // Custom hash combination function
    inline std::size_t hash_combine(std::size_t seed, std::size_t value)
    {
        return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
    }

    // Define a hash function for coordinates
    namespace std
    {
        template <typename T>
        struct hash<coordinates<T>>
        {
            std::size_t operator()(const coordinates<T>& coord) const
            {
                std::size_t h1 = std::hash<T>()(coord.x);
                std::size_t h2 = std::hash<T>()(coord.y);
                return hash_combine(h1, h2);
            }
        };
    }

#ifdef WIN64
    std::string wstring_to_string(const std::wstring& to_convert);
    std::wstring string_to_wstring(const std::string& to_convert);
#endif
}