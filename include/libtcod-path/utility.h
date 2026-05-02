#pragma once

/// @brief Convert a generic number to its absolute value.
#define TCODPATH_ABS(x) ((x) < 0 ? -(x) : (x))

#define TCODPATH_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define TCODPATH_MIN(a, b) (((a) < (b)) ? (a) : (b))
