#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue_addition.h"

static inline void swap(struct list_head *node_1, struct list_head *node_2)
{
    if (node_1 == node_2)
        return;
    struct list_head *node_2_prev = node_2->prev;
    list_del_init(node_2);
    list_splice_init(node_1, node_2);
    list_add(node_1, node_2_prev);
}

bool q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return false;
    struct list_head *ptr = head;
    int len = q_size(head);
    while (len > 0) {
        int rindex = rand() % (len);
        struct list_head *last = ptr->prev;
        struct list_head *node = head->next;
        while (rindex--) {
            node = node->next;
        }
        swap(last, node);
        ptr = ptr->prev;
        len--;
    }
    return true;
}