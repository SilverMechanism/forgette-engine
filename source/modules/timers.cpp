module;
#include "windows.h"
export module timers;
import std;

struct Timer
{
	int handle = -1;
	float remaining = 0;
	std::function<void()> callback;
	int calls = 1;
};

export class TimerManager
{
private:
	std::vector<Timer> Timers;
	int running_handle = 0;
	LARGE_INTEGER last_runtime;
	LARGE_INTEGER frequency;
	float delta_time;
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

	int CreateTimer(float duration, std::function<void()> callback)
	{
		Timers.push_back(Timer{ running_handle, duration, callback, 1 });
		++running_handle;
		return running_handle - 1;
	}

	int CreateTimer(float duration, std::function<void()> callback, int calls)
	{
		Timers.push_back(Timer{ running_handle, duration, callback, calls });
		++running_handle;
		return running_handle - 1;
	}

	void ParseTimers()
	{
		LARGE_INTEGER new_runtime;
		QueryPerformanceCounter(&new_runtime);
		LONGLONG ll_delta_time = (new_runtime.QuadPart - last_runtime.QuadPart);
		delta_time = static_cast<float>(ll_delta_time)/frequency.QuadPart;
		float interval = delta_time;

		Timers.erase(std::remove_if(Timers.begin(), Timers.end(),
			[interval, this](Timer& timer) {
				timer.remaining -= interval;
				if (timer.remaining <= 0)
				{
					CallbackQueue.push_back(timer.callback);
					--timer.calls;
					if (timer.calls <= 0)
					{
						return true;
					}
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

	void Initialize()
	{
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&last_runtime);
	}

	float get_delta_time()
	{
		return delta_time;
	}

private:
	TimerManager()
	{

	}
};