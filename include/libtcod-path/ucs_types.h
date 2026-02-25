#pragma once

#include "graph_types.h"
#include "heapq_types.h"
#include "map_types.h"

typedef struct TCODPATH_UniformCostSearch {
  int dimensions;
  struct TCODPATH_Heap frontier;
  TCODPATH_Graph* __restrict graph;
  TCODPATH_Map* __restrict distance;
  TCODPATH_Map* __restrict flow;
} TCODPATH_UniformCostSearch;
