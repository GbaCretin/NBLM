#ifndef MATH_HPP
#define MATH_HPP

#include <algorithm>

namespace math {
    template <typename T>
    inline const T& clamp(const T& value, const T& low, const T& high)
    {
        return std::min(std::max(value, low), high);
    }
}

#endif // MATH_HPP
