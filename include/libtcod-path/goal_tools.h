#pragma once

#include "goal_types.h"
#include "map_tools.h"

static inline bool TCODPATH_goal_is_reached(
    const TCODPATH_Goal* __restrict goal, int dimensions, const TCODPATH_IndexType* __restrict index) {
  if (!goal) return false;
  switch (goal->type) {
    case TCODPATH_GOAL_CALLBACK:
      return goal->callback.callback(goal->callback.userdata, dimensions, index);
    case TCODPATH_GOAL_TARGET:
      for (int i = 0; i < dimensions; ++i) {
        if (index[i] != goal->target.target[i]) return false;
      }
      return true;
    default:
      return false;
  }
}
