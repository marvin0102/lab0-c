#pragma once

#include <stdint.h>
#define ITERATIONS 100000


#define Q 8

#define INT2Q(x) x < 0 ? (uint32_t) x << Q | (1U << 31) : x << Q
typedef int32_t Q15_16;

int mcts(char *table, char player);
