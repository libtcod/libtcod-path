#pragma once

#include "config.h"

typedef TCODPATH_ValueType TCODPATH_HeuristicFunction(
    void* userdata, int dimensions, const TCODPATH_IndexType* __restrict index, TCODPATH_ValueType distance);

typedef enum TCODPATH_HeuristicTypes {
  TCODPATH_HEURISTIC_UNDEFINED = 0,
  TCODPATH_HEURISTIC_CALLBACK = 1,
  TCODPATH_HEURISTIC_BASIC = 2,
} TCODPATH_HeuristicTypes;

/// @brief A heuristic from a callback function.
struct TCODPATH_HeuristicCallback {
  int type;
  TCODPATH_HeuristicFunction* callback;
  void* userdata;
};

/// @brief A standard `A*`-like heuristic based on distance from a target.
struct TCODPATH_HeuristicBasic {
  int type;
  TCODPATH_IndexType target[TCODPATH_MAX_DIMENSIONS];
  TCODPATH_ValueType greed[TCODPATH_MAX_DIMENSIONS];
};

/// @brief Generic heuristic.
typedef union TCODPATH_Heuristic {
  int type;
  struct TCODPATH_HeuristicCallback callback;
  struct TCODPATH_HeuristicBasic basic;
} TCODPATH_Heuristic;
