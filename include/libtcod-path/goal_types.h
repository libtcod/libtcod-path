#pragma once

#include <stdbool.h>

#include "config.h"

typedef bool (*TCODPATH_GoalCallbackFunction)(
    void* userdata, int dimensions, const TCODPATH_IndexType* __restrict index);

typedef enum TCODPATH_GoalTypes {
  TCODPATH_GOAL_UNDEFINED = 0,
  TCODPATH_GOAL_CALLBACK = 1,
  TCODPATH_GOAL_TARGET = 2,
} TCODPATH_GoalTypes;

struct TCODPATH_GoalCallback {
  int type;  // Must be TCODPATH_GOAL_CALLBACK
  TCODPATH_GoalCallbackFunction callback;
  void* userdata;
};
struct TCODPATH_GoalTarget {
  int type;  // Must be TCODPATH_GOAL_TARGET
  TCODPATH_IndexType target[TCODPATH_MAX_DIMENSIONS];
};

typedef union TCODPATH_Goal {
  int type;
  struct TCODPATH_GoalCallback callback;
  struct TCODPATH_GoalTarget target;
} TCODPATH_Goal;
