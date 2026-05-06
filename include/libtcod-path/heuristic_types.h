#pragma once

#include "config.h"
#include "map_types.h"

typedef TCODPATH_ValueType TCODPATH_HeuristicFunction(
    void* userdata, int dimensions, const TCODPATH_IndexType* __restrict index, TCODPATH_ValueType distance);

typedef enum TCODPATH_HeuristicTypes {
  TCODPATH_HEURISTIC_UNDEFINED = 0,
  TCODPATH_HEURISTIC_CALLBACK = 1,
  TCODPATH_HEURISTIC_BASIC = 2,
  TCODPATH_HEURISTIC_DIFFERENTIAL = 3,
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

/// @brief A standard `A*`-like heuristic based on distance from a target.
struct TCODPATH_HeuristicDifferential {
  int type;
  TCODPATH_Map* __restrict differentials;
  int start_index;
  int end_index;
  TCODPATH_IndexType target[TCODPATH_MAX_DIMENSIONS];
  bool undirected;
};

/// @brief Generic heuristic.
typedef union TCODPATH_Heuristic {
  int type;
  struct TCODPATH_HeuristicCallback callback;
  struct TCODPATH_HeuristicBasic basic;
  struct TCODPATH_HeuristicDifferential differential;
} TCODPATH_Heuristic;
