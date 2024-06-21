module;
#include <cassert>
export module core:smart_pointers;

import std;

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
        keeper() noexcept : _depot(nullptr) {}
        
        // From raw pointer
        explicit keeper(T* new_pointer) : _depot(new depot<T>(new_pointer)) {}
        
        // Move
        keeper(keeper&& other) noexcept : _depot(other._depot)
        {
            other._depot = nullptr;
        }
        
        // Destruct
        ~keeper()
        {
            if (_depot)
            {
                if (!_depot->detach_keeper())
                {
                    delete _depot;
                }
            }

            _depot = nullptr;
        }
        
        // Move assignment
        keeper& operator=(keeper&& other) noexcept
        {
            if (this != &other)
            {
                if (_depot && !_depot->detach_keeper())
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
	class watcher {
	public:
	    friend class depot<T>;
	    friend class watcher;
	
	    // Default constructor
	    watcher() : _depot(nullptr) {}
	
	    // Construct by getting a depot from a keeper and watch it
	    explicit watcher(const keeper<T>& to_watch) : _depot(to_watch._depot) {
	        _depot->attach_watcher();
	    }
	
	    // Construct by creating a new copy of another watcher
	    explicit watcher(const watcher<T>& other) : _depot(other._depot) {
	        if (_depot) {
	            _depot->attach_watcher();
	        }
	    }
	
	    // Move constructor
	    watcher(watcher<T>&& other) noexcept : _depot(other._depot) {
	        other._depot = nullptr;
	    }
	
	    // Provide a swap member function
	    void swap(watcher<T>& other) noexcept {
	        std::swap(_depot, other._depot);
	    }
	
	    watcher<T>& operator=(watcher<T>&& other) noexcept {
	        if (this != &other) {
	            detach();
	            _depot = other._depot;
	            other._depot = nullptr;
	        }
	        return *this;
	    }
	
	    template <typename U>
	    explicit watcher(const keeper<U>& to_watch) {
	        static_assert(std::is_base_of<U, T>::value || std::is_base_of<T, U>::value || std::is_same<U, T>::value,
	                      "T and U must be in an inheritance relationship or be the same type");
	        _depot = reinterpret_cast<depot<T>*>(to_watch._depot);
	        if (_depot) {
	            _depot->attach_watcher();
	        } else {
	            std::cerr << "Urgghhnnn...\n";
	            assert(false);
	        }
	    }
	
	    template <typename U>
	    explicit watcher(const watcher<U>& to_watch) {
	        static_assert(std::is_base_of<U, T>::value || std::is_base_of<T, U>::value || std::is_same<U, T>::value,
	                      "T and U must be in an inheritance relationship or be the same type");
	        _depot = reinterpret_cast<depot<T>*>(to_watch._depot);
	        if (_depot) {
	            _depot->attach_watcher();
	        } else {
	            std::cerr << "Urgghhnnn...\n";
	            assert(false);
	        }
	    }
	
	    T* get() const {
	        return _depot ? _depot->data : nullptr;
	    }
	
	    void detach() {
	        if (_depot != nullptr) {
	            if (!_depot->detach_watcher()) {
					delete _depot;
				}
	        }
	    }
	
	    void watch(const keeper<T>& to_watch) {
	        detach();
	        _depot = to_watch._depot;
	    }
	
	    template <typename U>
	    void watch(const keeper<U>& to_watch) {
	        detach();
	        _depot = dynamic_cast<depot<T>*>(to_watch._depot);
	        if (_depot) {
	            _depot->attach_watcher();
	        }
	    }
	
	    T* operator->() {
	        return get();
	    }
	
	    explicit operator bool() const {
	        return get() != nullptr;
	    }
	
	    ~watcher() {
	        detach();
	    }
	
	private:
	    depot<T>* _depot = nullptr;
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

// specializations
namespace std 
{
    template <typename T>
    void swap(ptr::watcher<T>& lhs, ptr::watcher<T>& rhs) noexcept 
    {
        lhs.swap(rhs);
    }
}