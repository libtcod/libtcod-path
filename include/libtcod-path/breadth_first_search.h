#pragma once

#include "breadth_first_search_types.h"
#include "graph_tools.h"
#include "map_tools.h"
#include "ring_buffer.h"

static inline void TCODPATH_bfs_set_edge(
    void* bfs_data_,
    const TCODPATH_IndexType* __restrict root_index,
    const TCODPATH_IndexType* __restrict leaf_index,
    TCODPATH_ValueType) {
  TCODPATH_BreadthFirstSearch* __restrict bfs_data = (TCODPATH_BreadthFirstSearch*)bfs_data_;
  const TCODPATH_ValueType distance_at_root = TCODPATH_map_get(bfs_data->distance, root_index);
  const TCODPATH_ValueType distance_at_leaf = TCODPATH_map_get(bfs_data->distance, leaf_index);
  const TCODPATH_ValueType total_distance = distance_at_root + 1;
  if (distance_at_leaf <= total_distance) return;  // This edge is not better than a previous edge
  TCODPATH_map_set(bfs_data->distance, leaf_index, total_distance);
  TCODPATH_ring_buffer_append(&bfs_data->frontier, sizeof(*leaf_index) * bfs_data->dimensions, leaf_index);
  if (bfs_data->flow) {
    ;
  }
}

static inline int TCODPATH_bfs_step(TCODPATH_BreadthFirstSearch* __restrict bfs_data) {
  if (!bfs_data) return TCODPATH_E_INVALID_ARGUMENT;
  if (bfs_data->frontier.used_bytes <= 0) return 1;  // Iteration complete

  TCODPATH_IndexType index[TCODPATH_MAX_DIMENSIONS];
  TCODPATH_ring_buffer_pop(&bfs_data->frontier, sizeof(*index) * bfs_data->dimensions, index);
  TCODPATH_graph_foreach_edge(bfs_data->graph, bfs_data->dimensions, index, TCODPATH_bfs_set_edge, bfs_data);
  return 0;  // Iteration continues
}

static inline void TCODPATH_bfs(
    TCODPATH_Graph* __restrict graph, TCODPATH_Map* __restrict distance, TCODPATH_Map* __restrict flow) {
  TCODPATH_BreadthFirstSearch bfs_data = {};
  const int dimensions = bfs_data.dimensions = TCODPATH_map_get_dimensions(distance);
  bfs_data.graph = graph;
  bfs_data.distance = distance;
  bfs_data.flow = flow;

  // Use non-max values of distance to initialize the frontier
  TCODPATH_IndexType index[TCODPATH_MAX_DIMENSIONS];
  for (TCODPATH_indexes_iter_begin(dimensions, index);
       TCODPATH_indexes_iter_step(dimensions, TCODPATH_map_get_shape(distance), index);) {
    if (TCODPATH_map_is_max(distance, index)) continue;
    TCODPATH_ring_buffer_append(&bfs_data.frontier, sizeof(*index) * dimensions, index);
  }
  while (true) {
    int err = TCODPATH_bfs_step(&bfs_data);
    if (err != 0) break;
  }
  TCODPATH_ring_buffer_uninit(&bfs_data.frontier);
}
