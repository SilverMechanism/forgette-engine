export module core:core_math;

import std;

export namespace core_math
{
	template <typename T>
	T interp_linear(T value, T target, T step)
	{
		if (value > target)
		{
			value -= step;
			if (value < target)
			{
				value = target;
			}
		}
		else if (value < target)
		{
			value += step;
			if (value > target)
			{
				value = target;
			}
		}
		
		return value;
	}
	
	template <typename T>
	T interp_smooth(T value, T target, float factor) 
	{
	    // Ensure the factor is between 0 and 1
	    if (factor < 0.0f) factor = 0.0f;
	    if (factor > 1.0f) factor = 1.0f;
	
	    value += (target - value) * factor;
	    return value;
	}
	
	template <typename T>
	T interp_combo(T value, T target, float factor, float step)
	{
		value = interp_smooth(value, target, factor);
		return interp_linear(value, target, step);
	}
	
    template <class T>
    constexpr T pi = T(3.141592653589793);

    constexpr double tan(double x)
    {
        const double x2 = x * x;
        const double numerator = x * (135135.0 + x2 * (17325.0 + x2 * (378.0 + x2)));
        const double denominator = 135135.0 - x2 * (62370.0 + x2 * (3150.0 + x2 * (28.0)));
        return numerator / denominator;
    }

    constexpr double atan(double x)
    {
        constexpr double a1 = 0.999866;
        constexpr double a3 = -0.3302995;
        constexpr double a5 = 0.180141;
        constexpr double a7 = -0.085133;

        double x2 = x * x;
        return x * (a1 + a3 * x2 + a5 * x2 * x2 + a7 * x2 * x2 * x2);
    }

    constexpr double atan2(double y, double x)
    {
        if (x > 0)
        {
            return atan(y / x);
        }
        else if (x < 0 && y >= 0)
        {
            return atan(y / x) + pi<double>;
        }
        else if (x < 0 && y < 0)
        {
            return atan(y / x) - pi<double>;
        }
        else if (x == 0 && y > 0)
        {
            return pi<double> / 2.0;
        }
        else if (x == 0 && y < 0)
        {
            return -pi<double> / 2.0;
        }
        return 0.0; // x == 0 && y == 0
    }

    constexpr double to_degrees(double radians)
    {
        return radians * (180.0 / pi<double>);
    }

    constexpr double normalize_angle(double angle)
    {
        while (angle < 0)
        {
            angle += 2 * pi<double>;
        }
        while (angle >= 2 * pi<double>)
        {
            angle -= 2 * pi<double>;
        }
        return angle;
    }

    bool nearly_zero(float value, float epsilon = 1e-16f)
    {
        return std::fabs(value) < epsilon;
    }
}