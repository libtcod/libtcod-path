#pragma once

#include "config.h"
#include "map_types.h"

typedef void TCODPATH_GraphCallback(
    void* userdata, const int* __restrict root_index, const int* __restrict leaf_index, int edge_cost);

typedef enum TCODPATH_GraphTypes {
  TCODPATH_GRAPH_UNDEFINED = 0,
  // TCODPATH_GRAPH_CALLBACK = 1,
  TCODPATH_GRAPH_BASIC2D = 2,
  TCODPATH_GRAPH_STATIC = 3,
} TCODPATH_GraphTypes;

struct TCODPATH_GraphBasic2D {
  int type;  // Must be TCODPATH_GRAPH_BASIC2D
  TCODPATH_Map* __restrict map;  // Pointer to map costs
  int cardinal;  // Multiplier for cardinal costs, or 0 to disable cardinal movement
  int diagonal;  // Multiplier for diagonal costs, or 0 to disable diagonal movement
};

struct TCODPATH_GraphStatic {
  int type;  // Must be TCODPATH_GRAPH_STATIC
  TCODPATH_Map* __restrict map;  // Pointer to map costs
  int dimensions;
  int edge_count;
  /// An array of graph edges, must be a contiguous array of ``edges[edge_count * (dimensions + 1)]``
  int* __restrict edges;
};

typedef union TCODPATH_Graph {
  int type;
  struct TCODPATH_GraphBasic2D basic2d;
  struct TCODPATH_GraphStatic static_edges;
} TCODPATH_Graph;
