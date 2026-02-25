#pragma once

#include "graph_types.h"
#include "map_tools.h"

static inline void TCODPATH_graph_foreach_edge(
    TCODPATH_Graph* __restrict graph,
    int n,
    const int* __restrict index,
    TCODPATH_GraphCallback callback,
    void* userdata) {
  switch (graph->type) {
    case TCODPATH_GRAPH_BASIC2D: {
      int leaf_index[TCODPATH_MAX_DIMENSIONS];
      for (int i = 0; i < n; ++i) leaf_index[i] = index[i];  // Copy whole index, required for 3D+ graphs
      for (int y = -1; y <= 1; ++y) {  // Iterate over 3x3 grid surrounding index
        for (int x = -1; x <= 1; ++x) {
          if (x == 0 && y == 0) continue;  // Ignore center node
          leaf_index[n - 2] = index[n - 2] + y;
          leaf_index[n - 1] = index[n - 1] + x;
          const int base_cost = (x != 0 && y != 0) ? graph->basic2d.diagonal : graph->basic2d.cardinal;
          if (base_cost <= 0) continue;
          const int edge_cost = base_cost * TCODPATH_map_get(graph->basic2d.map, leaf_index);
          if (edge_cost <= 0) continue;
          callback(userdata, index, leaf_index, edge_cost);
        }
      }
    } break;
    case TCODPATH_GRAPH_STATIC:
      break;
    default:
      break;
  }
}
