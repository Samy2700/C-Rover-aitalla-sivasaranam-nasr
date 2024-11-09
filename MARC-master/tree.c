#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "stack.h"

t_treeNode* createTreeNode(t_localisation loc, int cost, t_move move, t_treeNode *parent) {
    t_treeNode *node = (t_treeNode *)malloc(sizeof(t_treeNode));
    node->loc = loc;
    node->cost = cost;
    node->move = move;
    node->children = NULL;
    node->num_children = 0;
    node->parent = parent;
    return node;
}

void addChild(t_treeNode *parent, t_treeNode *child) {
    parent->num_children++;
    parent->children = (t_treeNode **)realloc(parent->children, parent->num_children * sizeof(t_treeNode *));
    parent->children[parent->num_children - 1] = child;
}

void buildTree(t_treeNode *node, int depth, int max_depth, t_move *available_moves, int num_moves, t_map map, int level, int node_id) {
    if (depth >= max_depth) return;
    for (int i = 0; i < num_moves; i++) {
        t_localisation new_loc = move(node->loc, available_moves[i]);
        if (!isValidLocalisation(new_loc.pos, map.x_max, map.y_max)) continue;
        int move_cost = map.costs[new_loc.pos.y][new_loc.pos.x];
        t_treeNode *child = createTreeNode(new_loc, node->cost + move_cost, available_moves[i], node);
        addChild(node, child);
        printf("Niveau %d - Nœud %d: Mouvement %s, Position (%d, %d), Orientation %d, Coût cumulé %d\n",
               level, node_id, getMoveAsString(available_moves[i]), new_loc.pos.x, new_loc.pos.y, new_loc.ori, child->cost);
        buildTree(child, depth + 1, max_depth, available_moves, num_moves, map, level + 1, node_id * 10 + i);
    }
}

t_treeNode* findMinCostLeaf(t_treeNode *node, t_treeNode *min_node) {
    if (node->num_children == 0) {
        printf("Feuille atteinte : Coût cumulé %d\n", node->cost);
        if (min_node == NULL || node->cost < min_node->cost) {
            return node;
        }
        return min_node;
    }
    for (int i = 0; i < node->num_children; i++) {
        min_node = findMinCostLeaf(node->children[i], min_node);
    }
    return min_node;
}

void getPathFromLeaf(t_treeNode *leaf, t_move *path, int *path_length) {
    t_stack stack = createStack(10);
    t_treeNode *current = leaf;
    *path_length = 0;
    while (current->parent != NULL) {
        push(&stack, current->move);
        current = current->parent;
        (*path_length)++;
    }
    for (int i = 0; i < *path_length; i++) {
        printf("Top of stack before popping: %s\n", getMoveAsString(top(stack)));
        path[i] = pop(&stack);
    }
    free(stack.values);
}

void freeTree(t_treeNode *root) {
    if (root == NULL) return;
    for (int i = 0; i < root->num_children; i++) {
        freeTree(root->children[i]);
    }
    free(root->children);
    free(root);
}
