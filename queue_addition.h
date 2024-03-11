#include <stdbool.h>
#include <stddef.h>

#include "harness.h"
#include "list.h"
#include "queue.h"

bool q_shuffle(struct list_head *head);

typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);

void timsort(void *priv, struct list_head *head, list_cmp_func_t cmp);
