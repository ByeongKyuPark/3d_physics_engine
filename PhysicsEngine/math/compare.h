#pragma once
#include <functional>
namespace math {
	static std::function<bool(float, float)> Less = [](float v1, float v2) { return v1 < v2; };
	static std::function<bool(float, float)> Greater = [](float v1, float v2) { return v1 > v2; };
};
