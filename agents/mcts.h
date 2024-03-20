#pragma once

#include <stdint.h>
#define ITERATIONS 100000


#define Q 8
typedef int32_t Q23_8;

int mcts(char *table, char player);
