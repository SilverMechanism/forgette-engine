module;
#include <cassert>
#include <cwchar>
#ifdef WIN64
	#include <windows.h>
#endif
export module core;
import std;

static std::wstring exe_path;

export namespace core_math
{
	template <class T>
	constexpr T pi = T(3.141592653589793);
	
	constexpr double tan(double x)
    {
        const double x2 = x * x;
        const double numerator = x * (135135.0 + x2 * (17325.0 + x2 * (378.0 + x2)));
        const double denominator = 135135.0 - x2 * (62370.0 + x2 * (3150.0 + x2 * (28.0)));
        return numerator / denominator;
    }
}

std::wstring get_exe_path()
{
	return exe_path;
}

std::wstring get_exe_dir()
{
	std::wstring exe_dir = exe_path;
	
	int i = 0;
	for (i = static_cast<int>(exe_dir.size()); i > -1; i--)
	{
		if (exe_dir[i] == '\\')
		{
			break;
		}
	}
	
	i = static_cast<int>(exe_dir.size()) - i;
	
	for (i; i > 0; i--)
	{
		exe_dir.pop_back();
	}
	
	return exe_dir + L"\\";
}

void set_exe_path(std::wstring new_path)
{
	exe_path = new_path;
}

constexpr float tile_x = 256;
constexpr float tile_y = 128;

// Helper type trait to check if a type is in a list of types
template<typename T, typename... Types>
struct is_one_of : std::false_type {};

template<typename T, typename First, typename... Rest>
struct is_one_of<T, First, Rest...> : std::conditional_t<std::is_same_v<T, First>, std::true_type, is_one_of<T, Rest...>> {};

export 
{
	std::wstring get_exe_path();
	std::wstring get_exe_dir();
	void set_exe_path(std::wstring new_path);
	
	template <typename T>
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
		
		coordinates(T x1, T y1) :
			x(x1),
			y(y1)
		{
		
		}
		
		coordinates<float> isometric()
		{
			return {((x + y)*tile_x/2)/36, ((x - y)*tile_y/2)/36};
		}
		
		coordinates<float> world()
		{
			return {(x*36/tile_x/2)-y, (x*36/tile_y/2)+y};
		}
		
		coordinates<float> view_isometric()
		{
			return {(x - y), (x + y)};
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
		
		coordinates<float> get_facing(const coordinates<float> location, const coordinates<float> target)
		{
			coordinates<float> facing = {target.x - location.x, target.y - location.y};
			return facing.normalize();
		}
		
		template <typename U>
	    coordinates(const coordinates<U>& other) {
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
	    
	    bool operator==(const coordinates &other) const 
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
	    
	    coordinates<float> towards(coordinates<float> to_look_at)
	    {
	    	return coordinates<float>(to_look_at.x - this->x, to_look_at.y - this->y).normalize();
	    }
	};
	
	// Custom hash combination function
	inline std::size_t hash_combine(std::size_t seed, std::size_t value) {
	    return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
	}
	
	// Define a hash function for coordinates
	namespace std {
	    template <typename T>
	    struct hash<coordinates<T>> {
	        std::size_t operator()(const coordinates<T>& coord) const {
	            std::size_t h1 = std::hash<T>()(coord.x);
	            std::size_t h2 = std::hash<T>()(coord.y);
	            return hash_combine(h1, h2);
	        }
	    };
	}
	
#ifdef WIN64
	std::string wstring_to_string(const std::wstring &to_convert)
	{
		if (to_convert.empty()) return std::string();

	    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &to_convert[0], (int)to_convert.size(), NULL, 0, NULL, NULL);
	    std::string conversion(size_needed, 0);
	    WideCharToMultiByte(CP_UTF8, 0, &to_convert[0], (int)to_convert.size(), &conversion[0], size_needed, NULL, NULL);
	    return conversion;
	}
	
	std::wstring string_to_wstring(const std::string& to_convert) 
	{
	    std::mbstate_t state = std::mbstate_t();
	    const char* src = to_convert.c_str();
	    std::size_t len;
	    mbsrtowcs_s(&len, nullptr, 0, &src, 0, &state);
	    std::vector<wchar_t> wstr(len);
	    mbsrtowcs_s(&len, wstr.data(), len, &src, len, &state);
	    return std::wstring(wstr.data());
	}
#endif
}

export coordinates(float, float) -> coordinates<float>;
export coordinates(int, int) ->  coordinates<int>;

// * * * * * * * * //
//  Smart Pointer  //
// * * * * * * * * //

template <typename T>
class depot;

export namespace ptr
{
	template <typename T>
	class keeper;
	
	template <typename T>
	class watcher;
	
	// * * * //
	
	template <typename T>
	class keeper
	{
	public:
		template <typename U>
		friend class watcher;
		
		// Default (empty)
		keeper() noexcept : _depot(nullptr)
		{

		}
		
		// From raw pointer
		explicit keeper(T* new_pointer) : _depot(new depot<T>(new_pointer))
		{
			
		}
		
		// Move
		keeper(keeper&& other) noexcept : _depot(other._depot)
		{
			other._depot = nullptr;
		}
		
		// Destruct
		~keeper()
		{
			if (_depot && _depot->detach_keeper())
			{
				delete _depot;
			}
		}
	    
	    // Move assignment
	    keeper& operator=(keeper&& other) noexcept
	    {
	        if (this != &other)
	        {
	            if (_depot && _depot->detach_keeper())
	            {
	            	delete _depot;
	            }
	            
	            _depot = other._depot;
	            other._depot = nullptr;
	        }
	        return *this;
	    }
		
		T* get() const
		{
			return _depot ? _depot->data : nullptr;
		}
		
		T* operator->()
		{
			return get();
		}
	    
	private:
		depot<T>* _depot = nullptr;
		
		keeper(const keeper&) = delete;
        keeper& operator=(const keeper&) = delete;
	};
	
	template <typename T>
	class watcher
	{
	public:
		friend class depot<T>;
		friend class watcher;
		
		// Create empty watcher
		watcher() : _depot(nullptr)
		{
			
		}
		
		// Construct by getting a depot from a keeper and watch it
		explicit watcher(const keeper<T> &to_watch) : _depot(to_watch._depot)
		{
			_depot->attach_watcher();
		}
		
		// Construct by creating a new copy of another watcher
		explicit watcher(const watcher<T> &other) : _depot(other._depot)
		{
			if (_depot)
			{
				_depot->attach_watcher();
			}
		}
		
		
		explicit watcher(watcher<T> &&other) noexcept : _depot(other._depot) 
		{
			if (_depot)
			{
				_depot->attach_watcher();
			}
    	}
    	
    	watcher<T>& operator=(watcher<T> &&other) noexcept 
    	{
	        if (this != &other) 
	        {
	            detach();
	            _depot = other._depot;
	            
	            if (_depot)
	            {
	            	_depot->attach_watcher();
	            }
	        }
	        return *this;
    	}
		
		template <typename U>
	    explicit watcher(const keeper<U>& to_watch)
	    {
			static_assert(std::is_base_of<U, T>::value || std::is_base_of<T, U>::value || std::is_same<U, T>::value,
				"T and U must be in an inheritance relationship or be the same type");
			_depot = reinterpret_cast<depot<T>*>(to_watch._depot);
			if (_depot) {
				_depot->attach_watcher();
			}
			else {
				std::cerr << "Urgghhnnn...\n";
				assert(false);
			}
	    }
	    
	    template <typename U>
	    explicit watcher(const watcher<U>& to_watch)
	    {
			static_assert(std::is_base_of<U, T>::value || std::is_base_of<T, U>::value || std::is_same<U, T>::value,
				"T and U must be in an inheritance relationship or be the same type");
			_depot = reinterpret_cast<depot<T>*>(to_watch._depot);
			if (_depot) {
				_depot->attach_watcher();
			}
			else {
				std::cerr << "Urgghhnnn...\n";
				assert(false);
			}
	    }
	    
		T* get() const
		{
			return _depot ? _depot->data : nullptr;
		}
		
		void detach()
		{
			if (_depot != nullptr)
			{
				if (!_depot->detach_watcher())
				{
					delete _depot;
				}
			}
		}
		
		void watch(const keeper<T> &to_watch)
		{
			detach();
			_depot = to_watch._depot;
		}
		
		template <typename U>
	    void watch(const keeper<U>& to_watch)
	    {
	        detach();
	        _depot = dynamic_cast<depot<T>*>(to_watch._depot);
	        if (_depot) {
	            _depot->attach_watcher();
	        }
	    }
		
		T* operator->()
		{
			return get();
		}
		
		explicit operator bool() const
		{
			return get() != nullptr;
		}
		
		~watcher()
		{
			detach();
		}
		
	private:
		depot<T>* _depot = nullptr;
		
		// watcher(const watcher&) = delete;
        // watcher& operator=(const watcher&) = delete;
	};
}

template <typename T>
class depot
{
public:
	virtual ~depot() {}
	
	explicit depot(T* new_data) : data(new_data) {}
	
	T* data = nullptr;
	int watcher_count = 0;
	
	bool detach_keeper()
	{
		if (data != nullptr)
		{
			delete data;
			data = nullptr;
		}
		
		return (watcher_count);
	}
	
	bool detach_watcher()
	{
		watcher_count--;
		return (data || watcher_count);
	}
	
	void attach_watcher()
	{
		watcher_count++;
	}
	
	T* get_data()
	{
		return data;
	}
	
	// virtual depot<T>* dynamic_cast_depot() {return this;}
};