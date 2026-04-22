#pragma once

#ifndef TCODPATH_MAX_DIMENSIONS
/// @brief Maximum number of dimensions for map types.
/// Algorithms will typically support 1 less than what is defined here.
#define TCODPATH_MAX_DIMENSIONS 5
#endif

#ifndef TCODPATH_ValueType
/// @brief Type for map values.
#define TCODPATH_ValueType int
#endif

#ifndef TCODPATH_IndexType
/// @brief Type for indexes.
#define TCODPATH_IndexType int
#endif
