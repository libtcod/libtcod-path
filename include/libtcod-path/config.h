#pragma once
#include <limits.h>
#include <stdint.h>

#ifndef TCODPATH_MAX_DIMENSIONS
/// @brief Maximum number of dimensions for map types.
/// Algorithms will typically support 1 less than what is defined here.
#define TCODPATH_MAX_DIMENSIONS 5
#endif

#ifndef TCODPATH_ValueType
/// @brief Type for map values.
/// If this is manually set then `TCODPATH_VALUE_MAX` and `TCODPATH_VALUE_MIN` must also be set.
#define TCODPATH_ValueType int
#define TCODPATH_VALUE_MAX INT_MAX
#define TCODPATH_VALUE_MIN INT_MIN
#endif

#ifndef TCODPATH_IndexType
/// @brief Type for indexes.
#define TCODPATH_IndexType int
#endif
