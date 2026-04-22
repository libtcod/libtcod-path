#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "graph_tools.h"
#include "graph_types.h"
#include "map_tools.h"
#include "map_types.h"
#include "ring_buffer.h"

static inline void TCODPATH_partition_set_bool_if_open(
    void* userdata, const TCODPATH_IndexType*, const TCODPATH_IndexType*, TCODPATH_ValueType) {
  *(bool*)userdata = true;
}

struct TCODPATH_FloodFill_ {
  TCODPATH_RingBuffer frontier;
  TCODPATH_Graph* graph;
  TCODPATH_Map* __restrict map;
  TCODPATH_ValueType value;
};

static inline void TCODPATH_partition_flood_fill(
    void* userdata, const TCODPATH_IndexType*, const TCODPATH_IndexType* __restrict leaf_index, TCODPATH_ValueType) {
  struct TCODPATH_FloodFill_* data = (struct TCODPATH_FloodFill_*)userdata;
  if (TCODPATH_map_get(data->map, leaf_index) != 0) return;
  TCODPATH_map_set(data->map, leaf_index, data->value);
  TCODPATH_ring_buffer_append(
      &data->frontier, sizeof(*leaf_index) * TCODPATH_map_get_dimensions(data->map), leaf_index);
}

int TCODPATH_partition_from_graph(TCODPATH_Graph* __restrict graph, TCODPATH_Map* __restrict out) {
  const int dimensions = TCODPATH_map_get_dimensions(out);
  const TCODPATH_IndexType* shape = TCODPATH_map_get_shape(out);
  TCODPATH_IndexType index[TCODPATH_MAX_DIMENSIONS];

  // Clear map
  for (TCODPATH_indexes_iter_begin(dimensions, index); TCODPATH_indexes_iter_step(dimensions, shape, index);) {
    TCODPATH_map_set(out, index, 0);
  }
  // Partition map
  TCODPATH_ValueType total_partitions = 0;
  struct TCODPATH_FloodFill_ flood_fill_data = {};
  for (TCODPATH_indexes_iter_begin(dimensions, index); TCODPATH_indexes_iter_step(dimensions, shape, index);) {
    if (TCODPATH_map_get(out, index) != 0) continue;  // Partition already known for this index
    bool is_open = false;
    TCODPATH_graph_foreach_edge(graph, dimensions, index, TCODPATH_partition_set_bool_if_open, (void*)&is_open);
    if (!is_open) continue;  // Node has no edges
    ++total_partitions;
    flood_fill_data.graph = graph;
    flood_fill_data.map = out;
    flood_fill_data.value = total_partitions;
    TCODPATH_partition_flood_fill(&flood_fill_data, NULL, index, 0);
    while (flood_fill_data.frontier.used) {  // Until ring buffer is empty
      TCODPATH_IndexType next_index[TCODPATH_MAX_DIMENSIONS];
      TCODPATH_ring_buffer_pop(&flood_fill_data.frontier, sizeof(*index) * dimensions, next_index);
      TCODPATH_map_set(out, next_index, total_partitions);
      TCODPATH_graph_foreach_edge(
          graph, dimensions, next_index, TCODPATH_partition_flood_fill, (void*)&flood_fill_data);
    }
  }
  TCODPATH_ring_buffer_uninit(&flood_fill_data.frontier);
  return total_partitions;
}
