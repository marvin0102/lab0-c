#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "game.h"
#include "mcts.h"
#include "util.h"

#define EXPLORATION_FACTOR fixed_sqrt(2 << Q)
#define K (1 << (Q - 1))

struct node {
    int move;
    char player;
    int n_visits;
    Q15_16 score;
    struct node *parent;
    struct node *children[N_GRIDS];
};

static struct node *new_node(int move, char player, struct node *parent)
{
    struct node *node = malloc(sizeof(struct node));
    node->move = move;
    node->player = player;
    node->n_visits = 0;
    node->score = 0;
    node->parent = parent;
    memset(node->children, 0, sizeof(node->children));
    return node;
}

static void free_node(struct node *node)
{
    for (int i = 0; i < N_GRIDS; i++)
        if (node->children[i])
            free_node(node->children[i]);
    free(node);
}

Q15_16 fixed_sqrt(Q15_16 x)
{
    if (x <= 1 << Q) /* Assume x is always positive */
        return x;

    Q15_16 z = 0;
    for (Q15_16 m = 1UL << ((31 - __builtin_clz(x)) & ~1UL); m; m >>= 2) {
        int b = z + m;
        z >>= 1;
        if (x >= b)
            x -= b, z += m;
    }
    z = z << Q / 2;
    return z;
}


Q15_16 fixed_log(int input)
{
    if (!input || input == (1U << Q))
        return 0;

    int64_t y = input << Q;  // int to Q15_16
    y = ((y - (1U << Q)) << (Q)) / (y + (1U << Q));
    int64_t ans = 0U;
    for (unsigned i = 1; i < 20; i += 2) {
        int64_t z = (1U << Q);
        for (int j = 0; j < i; j++) {
            z *= y;
            z >>= Q;
        }
        z <<= Q;
        z /= (i << Q);

        ans += z;
    }
    ans <<= 1;
    return (Q15_16) ans;
}

static inline Q15_16 uct_score(int n_total, int n_visits, Q15_16 score)
{
    if (n_visits == 0)
        return INT32_MAX;
    Q15_16 result = score << Q / (Q15_16) (n_visits << Q);
    int64_t tmp =
        EXPLORATION_FACTOR * fixed_sqrt(fixed_log(n_total) / n_visits);
    Q15_16 resultN = tmp >> Q;
    return result + resultN;
}

static struct node *select_move(struct node *node)
{
    struct node *best_node = NULL;
    Q15_16 best_score = INT2Q(-1);
    for (int i = 0; i < N_GRIDS; i++) {
        if (!node->children[i])
            continue;
        Q15_16 score = uct_score(node->n_visits, node->children[i]->n_visits,
                                 node->children[i]->score);
        if (score > best_score) {
            best_score = score;
            best_node = node->children[i];
        }
    }
    return best_node;
}

static Q15_16 simulate(char *table, char player)
{
    char current_player = player;
    char temp_table[N_GRIDS];
    memcpy(temp_table, table, N_GRIDS);
    while (1) {
        int *moves = available_moves(temp_table);
        if (moves[0] == -1) {
            free(moves);
            break;
        }
        int n_moves = 0;
        while (n_moves < N_GRIDS && moves[n_moves] != -1)
            ++n_moves;
        int move = moves[rand() % n_moves];
        free(moves);
        temp_table[move] = current_player;
        char win;
        if ((win = check_win(temp_table)) != ' ')
            return calculate_win_value(win, player);
        current_player ^= 'O' ^ 'X';
    }
    return 0.5;
}

static void backpropagate(struct node *node, Q15_16 score)
{
    while (node) {
        node->n_visits++;
        node->score += score;
        node = node->parent;
        score = 1 - score;
    }
}

static void expand(struct node *node, char *table)
{
    int *moves = available_moves(table);
    int n_moves = 0;
    while (n_moves < N_GRIDS && moves[n_moves] != -1)
        ++n_moves;
    for (int i = 0; i < n_moves; i++) {
        node->children[i] = new_node(moves[i], node->player ^ 'O' ^ 'X', node);
    }
    free(moves);
}

int mcts(char *table, char player)
{
    char win;
    struct node *root = new_node(-1, player, NULL);
    for (int i = 0; i < ITERATIONS; i++) {
        struct node *node = root;
        char temp_table[N_GRIDS];
        memcpy(temp_table, table, N_GRIDS);
        while (1) {
            if ((win = check_win(temp_table)) != ' ') {
                Q15_16 score =
                    calculate_win_value(win, node->player ^ 'O' ^ 'X');
                backpropagate(node, score);
                break;
            }
            if (node->n_visits == 0) {
                Q15_16 score = simulate(temp_table, node->player);
                backpropagate(node, score);
                break;
            }
            if (node->children[0] == NULL)
                expand(node, temp_table);
            node = select_move(node);
            assert(node);
            temp_table[node->move] = node->player ^ 'O' ^ 'X';
        }
    }
    struct node *best_node = NULL;
    int most_visits = -1;
    for (int i = 0; i < N_GRIDS; i++) {
        if (root->children[i] && root->children[i]->n_visits > most_visits) {
            most_visits = root->children[i]->n_visits;
            best_node = root->children[i];
        }
    }
    int best_move = -1;
    if (best_node)
        best_move = best_node->move;
    free_node(root);
    return best_move;
}
