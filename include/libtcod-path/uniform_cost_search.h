#pragma once

#include "graph_tools.h"
#include "graph_types.h"
#include "heapq_tools.h"
#include "map_tools.h"
#include "map_types.h"
#include "uniform_cost_search_types.h"

static inline int TCODPATH_ucs_heuristic_at(
    TCODPATH_UniformCostSearch* __restrict ucs_data, const int* __restrict index, int distance) {
  return distance;
}

static inline void TCODPATH_ucs_set_edge(
    TCODPATH_UniformCostSearch* __restrict ucs_data,
    const int* __restrict root_index,
    const int* __restrict leaf_index,
    int edge_cost) {
  const int distance_at_root = TCODPATH_map_get(ucs_data->distance, root_index);
  const int distance_at_leaf = TCODPATH_map_get(ucs_data->distance, leaf_index);
  const int total_distance = distance_at_root + edge_cost;
  if (distance_at_leaf <= total_distance) return;  // This edge is not better than a previous edge
  TCODPATH_map_set(ucs_data->distance, leaf_index, total_distance);
  TCODPATH_minheap_push(
      &ucs_data->frontier, TCODPATH_ucs_heuristic_at(ucs_data, leaf_index, total_distance), leaf_index);
  if (ucs_data->flow) {
    ;
  }
}

/// @brief Edge callback version of TCODPATH_ucs_set_edge, for passing to TCODPATH_graph_foreach_edge
static inline void TCODPATH_ucs_set_edge_callback(
    void* __restrict ucs_data, const int* __restrict root_index, const int* __restrict leaf_index, int edge_cost) {
  TCODPATH_ucs_set_edge((TCODPATH_UniformCostSearch* __restrict)ucs_data, root_index, leaf_index, edge_cost);
}

static inline int TCODPATH_ucs_step(TCODPATH_UniformCostSearch* __restrict ucs_data) {
  if (!ucs_data) return TCODPATH_E_INVALID_ARGUMENT;
  if (ucs_data->frontier.size <= 0) return 1;

  int index[TCODPATH_MAX_DIMENSIONS];
  TCODPATH_minheap_pop(&ucs_data->frontier, index);
  TCODPATH_graph_foreach_edge(ucs_data->graph, ucs_data->dimensions, index, TCODPATH_ucs_set_edge_callback, ucs_data);
  return 0;
}

static inline void TCODPATH_dijkstra(
    TCODPATH_Graph* __restrict graph, TCODPATH_Map* __restrict distance, TCODPATH_Map* __restrict flow) {
  TCODPATH_UniformCostSearch ucs_data = {0};
  const int dimensions = ucs_data.dimensions = TCODPATH_map_get_dimensions(distance);
  TCODPATH_heap_init(&ucs_data.frontier, dimensions * sizeof(int));
  ucs_data.graph = graph;
  ucs_data.distance = distance;
  ucs_data.flow = flow;

  // Use non-max values of distance to initialize the frontier
  int index[TCODPATH_MAX_DIMENSIONS];
  for (TCODPATH_indexes_iter_begin(dimensions, index);
       TCODPATH_indexes_iter_step(dimensions, TCODPATH_map_get_shape(distance), index);) {
    if (TCODPATH_map_is_max(distance, index)) continue;
    const int distance_here = TCODPATH_map_get(distance, index);
    TCODPATH_minheap_push(&ucs_data.frontier, TCODPATH_ucs_heuristic_at(&ucs_data, index, distance_here), index);
  }
  while (true) {
    int err = TCODPATH_ucs_step(&ucs_data);
    if (err != 0) break;
  }
  TCODPATH_heap_uninit(&ucs_data.frontier);
}
