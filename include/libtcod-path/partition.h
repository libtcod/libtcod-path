#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "graph_tools.h"
#include "graph_types.h"
#include "map_tools.h"
#include "map_types.h"

static inline void TCODPATH_partition_set_bool_if_open(void* userdata, const int*, const int*, int) {
  *(bool*)userdata = true;
}

struct TCODPATH_FloodFill_ {
  TCODPATH_Graph* graph;
  TCODPATH_Map* __restrict map;
  int value;
};

static inline void TCODPATH_partition_depth_first_flood_fill(
    void* userdata, const int*, const int* __restrict leaf_index, int) {
  struct TCODPATH_FloodFill_* data = (struct TCODPATH_FloodFill_*)userdata;
  if (TCODPATH_map_get(data->map, leaf_index) != 0) return;
  TCODPATH_map_set(data->map, leaf_index, data->value);
  TCODPATH_graph_foreach_edge(
      data->graph,
      TCODPATH_map_get_dimensions(data->map),
      leaf_index,
      TCODPATH_partition_depth_first_flood_fill,
      userdata);
}

int TCODPATH_partition_from_graph(TCODPATH_Graph* __restrict graph, TCODPATH_Map* __restrict out) {
  const int dimensions = TCODPATH_map_get_dimensions(out);
  const int* shape = TCODPATH_map_get_shape(out);
  int index[TCODPATH_MAX_DIMENSIONS];

  // Clear map
  for (TCODPATH_indexes_iter_begin(dimensions, index); TCODPATH_indexes_iter_step(dimensions, shape, index);) {
    TCODPATH_map_set(out, index, 0);
  }
  // Partition map
  int total_partitions = 0;
  for (TCODPATH_indexes_iter_begin(dimensions, index); TCODPATH_indexes_iter_step(dimensions, shape, index);) {
    if (TCODPATH_map_get(out, index) != 0) continue;  // Partition already known for this index
    bool is_open = false;
    TCODPATH_graph_foreach_edge(graph, dimensions, index, TCODPATH_partition_set_bool_if_open, (void*)&is_open);
    if (!is_open) continue;  // Node has no edges
    ++total_partitions;
    struct TCODPATH_FloodFill_ flood_fill_data = {
        graph,
        out,
        total_partitions,
    };
    // TODO: switch this to something which won't stack overflow
    TCODPATH_map_set(out, index, total_partitions);
    TCODPATH_graph_foreach_edge(
        graph, dimensions, index, TCODPATH_partition_depth_first_flood_fill, (void*)&flood_fill_data);
  }
  return total_partitions;
}
