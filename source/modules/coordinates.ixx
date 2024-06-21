export module core:coordinates;

import std;
import :core_math;

constexpr float tile_x = 256;
constexpr float tile_y = 128;
constexpr float tile_size = 36;

// Helper type trait to check if a type is in a list of types
template<typename T, typename... Types>
struct is_one_of : std::false_type {};

template<typename T, typename First, typename... Rest>
struct is_one_of<T, First, Rest...> : std::conditional_t<std::is_same_v<T, First>, std::true_type, is_one_of<T, Rest...>> {};

export template <typename T>
struct coordinates
{
    // Specialize for the list of allowed types
    using allowed_coordinate_types = is_one_of<T, 
        std::uint8_t, std::int8_t, std::uint16_t, std::int16_t, std::uint32_t, std::int32_t, 
        std::uint64_t, std::int64_t, float, double>;

    static_assert(allowed_coordinate_types::value, "Coordinates must be float, double, or specified integer type (eg std::uint16_t)");

    T x = static_cast<T>(0);
    T y = static_cast<T>(0);

    coordinates() = default;

    coordinates(T x1, T y1) : x(x1), y(y1) {}

    coordinates<float> isometric()
    {
        return {((x + y)*tile_x/2)/tile_size, ((x - y)*tile_y/2)/tile_size};
    }

    coordinates<float> world()
    {
        float A = (x * tile_size * 2) / tile_x;
        float B = (y * tile_size * 2) / tile_y;
        
        return { (A + B) / 2, (A - B) / 2 };
    }

    coordinates<float> view_isometric()
    {
        return {(x - y), (x + y)};
    }

    coordinates<float> z_shift(float z)
    {
        return {x, y - (z * 0.5f)};
    }

    float magnitude()
    {
        return std::sqrt(x * x + y * y);
    }

    coordinates<float> normalize()
    {
        float magnitude = this->magnitude();
        
        if (magnitude != 0) 
        {
            return {x/magnitude, y/magnitude};
        }
        else
        {
            return {0, 0};
        }
    }

    coordinates<float> get_facing(const coordinates<float>& location, const coordinates<float>& target)
    {
        coordinates<float> facing = {target.x - location.x, target.y - location.y};
        return facing.normalize();
    }

    template <typename U>
    coordinates(const coordinates<U>& other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
    }

    coordinates operator+(const coordinates& other) const
    {
        return {x + other.x, y + other.y};
    }

    coordinates operator-(const coordinates& other) const
    {
        return {x - other.x, y - other.y};
    }

    template <typename U>
    coordinates operator*(const U scalar) const
    {
        return coordinates(x * static_cast<T>(scalar), y * static_cast<T>(scalar));
    }

    coordinates operator*(const coordinates& other) const
    {
        return {x * other.x, y * other.y};
    }

    template <typename U>
    coordinates operator*(const coordinates<U>& other) const
    {
        return {x * static_cast<T>(other.x), y * static_cast<T>(other.y)};
    }

    template <typename U>
    coordinates<T>& operator=(const coordinates<U>& other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        return *this;
    }

    template <typename U>
    coordinates operator/(const U divisor) const
    {
        return coordinates(x/static_cast<T>(divisor), y/static_cast<T>(divisor));
    }

    // Returns false if x + y == 0
    explicit operator bool() const
    {
        if constexpr (std::is_same_v<T, int>)
        {
            return (x + y) != 0;
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            return (std::abs(x) + std::abs(y)) > std::numeric_limits<float>::epsilon();
        }
    }

    bool operator==(const coordinates& other) const
    {
        return x == other.x && y == other.y;
    }

    explicit operator std::string() const
    {
        return std::format("({}, {})", x, y);
    }

    coordinates<float> random_nearby(float radius)
    {
        static std::mt19937 generator(static_cast<unsigned int>(std::time(0))); // Seed the generator once
        std::uniform_real_distribution<float> distribution_angle(0.0f, 2.0f * core_math::pi<float>);
        std::uniform_real_distribution<float> distribution_radius(0.0f, radius);

        float angle = distribution_angle(generator);
        float random_radius = distribution_radius(generator);

        return {this->x + random_radius * std::cos(angle), this->y + random_radius * std::sin(angle)};
    }

    coordinates<float> towards(coordinates<float> to_look_at) const
    {
        return coordinates<float>(to_look_at.x - this->x, to_look_at.y - this->y).normalize();
    }
};