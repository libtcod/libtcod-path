#pragma once

#include "goal_tools.h"
#include "graph_tools.h"
#include "graph_types.h"
#include "heapq_tools.h"
#include "heuristic_tools.h"
#include "map_tools.h"
#include "map_types.h"
#include "uniform_cost_search_types.h"

static inline void TCODPATH_ucs_set_edge(
    void* ucs_data_,
    const TCODPATH_IndexType* __restrict root_index,
    const TCODPATH_IndexType* __restrict leaf_index,
    TCODPATH_ValueType edge_cost) {
  TCODPATH_UniformCostSearch* __restrict ucs_data = (TCODPATH_UniformCostSearch*)ucs_data_;
  const TCODPATH_ValueType distance_at_root = TCODPATH_map_get(ucs_data->distance, root_index);
  const TCODPATH_ValueType distance_at_leaf = TCODPATH_map_get(ucs_data->distance, leaf_index);
  const TCODPATH_ValueType total_distance = distance_at_root + edge_cost;
  if (distance_at_leaf <= total_distance) return;  // This edge is not better than a previous edge
  TCODPATH_map_set(ucs_data->distance, leaf_index, total_distance);
  TCODPATH_minheap_push(
      &ucs_data->frontier,
      TCODPATH_heuristic_at(ucs_data->heuristic, ucs_data->dimensions, leaf_index, total_distance),
      leaf_index);
  if (ucs_data->flow) TCODPATH_map_set_index(ucs_data->flow, leaf_index, root_index);
}

/// @brief Preform a single iteration of UCS. Return the status.
/// @return `1` when exhausted, `2` when goal reached, `0` when incomplete, negative value on error.
static inline int TCODPATH_ucs_step(TCODPATH_UniformCostSearch* __restrict ucs_data) {
  if (!ucs_data) return TCODPATH_E_INVALID_ARGUMENT;
  if (ucs_data->frontier.size <= 0) return 1;  // Iteration complete

  TCODPATH_IndexType index[TCODPATH_MAX_DIMENSIONS];
  if (TCODPATH_goal_is_reached(ucs_data->goal, ucs_data->dimensions, (TCODPATH_IndexType*)ucs_data->frontier.heap)) {
    return 2;  // Goal reached
  }
  TCODPATH_minheap_pop(&ucs_data->frontier, index);
  TCODPATH_graph_foreach_edge(ucs_data->graph, ucs_data->dimensions, index, TCODPATH_ucs_set_edge, ucs_data);
  return 0;  // Iteration continues
}
static inline int TCODPATH_ucs_compute(TCODPATH_UniformCostSearch* __restrict ucs_data, int max_iterations) {
  while (true) {
    int err = TCODPATH_ucs_step(ucs_data);
    if (err != 0) return err;
    if (max_iterations && --max_iterations == 0) return 0;  // Max iterations reached
  }
}

static inline void TCODPATH_dijkstra(
    TCODPATH_Graph* __restrict graph, TCODPATH_Map* __restrict distance, TCODPATH_Map* __restrict flow) {
  TCODPATH_UniformCostSearch ucs_data = {0};
  const int dimensions = ucs_data.dimensions = TCODPATH_map_get_dimensions(distance);
  TCODPATH_IndexType index[TCODPATH_MAX_DIMENSIONS];
  TCODPATH_heap_init(&ucs_data.frontier, dimensions * sizeof(*index));
  ucs_data.graph = graph;
  ucs_data.distance = distance;
  ucs_data.flow = flow;

  // Use non-max values of distance to initialize the frontier
  for (TCODPATH_indexes_iter_begin(dimensions, index);
       TCODPATH_indexes_iter_step(dimensions, TCODPATH_map_get_shape(distance), index);) {
    if (TCODPATH_map_is_max(distance, index)) continue;
    const TCODPATH_ValueType distance_here = TCODPATH_map_get(distance, index);
    TCODPATH_minheap_push(
        &ucs_data.frontier, TCODPATH_heuristic_at(ucs_data.heuristic, dimensions, index, distance_here), index);
  }
  TCODPATH_ucs_compute(&ucs_data, 0);
  TCODPATH_heap_uninit(&ucs_data.frontier);
}

static inline void TCODPATH_astar(
    TCODPATH_Graph* __restrict graph,
    TCODPATH_Map* __restrict distance,
    TCODPATH_Map* __restrict flow,
    const TCODPATH_IndexType* __restrict root_ij,
    const TCODPATH_IndexType* __restrict goal_ij) {
  TCODPATH_UniformCostSearch ucs_data = {0};
  const int dimensions = ucs_data.dimensions = TCODPATH_map_get_dimensions(distance);
  ucs_data.graph = graph;
  ucs_data.distance = distance;
  ucs_data.flow = flow;

  TCODPATH_Heuristic heuristic = {0};
  heuristic.type = TCODPATH_HEURISTIC_BASIC;
  for (int i = 0; i < dimensions; ++i) heuristic.basic.greed[i] = 1;

  TCODPATH_Goal goal = {0};
  goal.type = TCODPATH_GOAL_TARGET;

  for (int i = 0; i < dimensions; ++i) heuristic.basic.target[i] = goal.target.target[i] = goal_ij[i];

  ucs_data.heuristic = &heuristic;
  ucs_data.goal = &goal;

  TCODPATH_heap_init(&ucs_data.frontier, dimensions * sizeof(TCODPATH_IndexType));
  TCODPATH_minheap_push(&ucs_data.frontier, TCODPATH_heuristic_at(ucs_data.heuristic, dimensions, root_ij, 0), root_ij);
  TCODPATH_map_set(distance, root_ij, 0);

  TCODPATH_ucs_compute(&ucs_data, 0);
  TCODPATH_heap_uninit(&ucs_data.frontier);
}
