#pragma once

#include "goal_types.h"
#include "graph_types.h"
#include "map_types.h"
#include "ring_buffer.h"

/// @brief State for Breadth-cost search.
typedef struct TCODPATH_BreadthFirstSearch {
  int dimensions;
  TCODPATH_RingBuffer frontier;
  TCODPATH_Graph* __restrict graph;
  TCODPATH_Map* __restrict distance;
  TCODPATH_Map* __restrict flow;
  TCODPATH_Goal* __restrict goal;
} TCODPATH_BreadthFirstSearch;
