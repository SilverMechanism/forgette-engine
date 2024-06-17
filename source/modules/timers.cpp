module;
#include "windows.h"
export module timers;
import std;
import core;

struct Timer
{
	int handle = -1;
	float remaining = 0;
	float base = 0;
	std::function<void(int call)> callback;
	int calls = 1;
	int calls_made = 0;
};

export class TimerManager
{
private:
	std::vector<Timer> Timers;
	std::vector<Timer> new_timers;
	
	int running_handle = 0;
	LARGE_INTEGER last_runtime;
	double frequency = 0.0;
	double delta_time;
	std::vector<std::function<void()>> CallbackQueue;
public:
	static TimerManager* Instance()
	{
		static TimerManager instance;
		return &instance;
	}

	TimerManager(const TimerManager&) = delete;
	void operator=(const TimerManager&) = delete;

	// The pointer to this Timer is only valid until another timer expires.
	const Timer* GetTimer(int handle)
	{
		for (Timer& timer : Timers)
		{
			if (timer.handle == handle)
			{
				return &timer;
			}
		}
	}

	int CreateTimer(float duration, std::function<void(int call)> callback)
	{
		new_timers.push_back(Timer{ running_handle, duration, duration, callback, 1 });
		++running_handle;
		return running_handle - 1;
	}

	int CreateTimer(float duration, std::function<void(int calls)> callback, int calls)
	{
		new_timers.push_back(Timer{ running_handle, duration, duration, callback, calls });
		++running_handle;
		return running_handle - 1;
	}
	
	void Initialize()
	{
		LARGE_INTEGER qpf;
		QueryPerformanceFrequency(&qpf);
		frequency = double(qpf.QuadPart);
		QueryPerformanceCounter(&last_runtime);
	}

	void ParseTimers()
	{
		if (!new_timers.empty())
		{
			Timers.insert(
				Timers.end(),
				std::make_move_iterator(new_timers.begin()),
				std::make_move_iterator(new_timers.end()));

			new_timers.clear();
		}
		
		LARGE_INTEGER new_runtime;
		QueryPerformanceCounter(&new_runtime);
		delta_time = double(new_runtime.QuadPart - last_runtime.QuadPart)/frequency;
		// delta_time = static_cast<float>(ll_delta_time)/frequency.QuadPart;
		float interval = float(delta_time);

		Timers.erase(std::remove_if(Timers.begin(), Timers.end(),
			[interval, this](Timer& timer) {
				timer.remaining -= interval;
				if (timer.remaining <= 0)
				{
					// CallbackQueue.push_back(timer.callback);
					timer.callback(timer.calls_made);
					--timer.calls;
					++timer.calls_made;
					if (timer.calls <= 0)
					{
						return true;
					}
					timer.remaining = timer.base;
				}

				return false;
			}),
			Timers.end());

		last_runtime = new_runtime;
	}

	void ProcessCallbacks()
	{
		for (auto timer_callback : CallbackQueue)
		{
			timer_callback();
		}

		CallbackQueue.clear();
	}

	float get_delta_time()
	{
		return float(delta_time);
	}

private:
	TimerManager()
	{

	}
};