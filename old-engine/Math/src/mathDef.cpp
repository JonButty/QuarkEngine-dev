#include "mathDef.h"

#include <limits>

const float Math::PI = 3.1415926535f;
const float Math::TAU = 6.283185307179586476925286766559f;
const float Math::EPSILON = std::numeric_limits<float>::min();
const float Math::FLOAT_MAX = std::numeric_limits<float>::max();
const int Math::INTEGER_MIN = std::numeric_limits<int>::min();
const int Math::INTEGER_MAX = std::numeric_limits<int>::max();
const unsigned int Math::UINTEGER_MAX = std::numeric_limits<unsigned int>::max();