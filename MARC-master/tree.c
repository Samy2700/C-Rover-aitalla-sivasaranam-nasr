#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "stack.h"

// Crée un nœud de l'arbre
t_treeNode* createTreeNode(t_localisation loc, int cost, t_move move, t_treeNode *parent) {
    t_treeNode *node = (t_treeNode *)malloc(sizeof(t_treeNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for tree node\n");
        exit(1);
    }
    node->loc = loc;
    node->cost = cost;
    node->move = move;
    node->children = NULL;
    node->num_children = 0;
    node->parent = parent;
    return node;
}

// Ajoute un enfant à un nœud parent
void addChild(t_treeNode *parent, t_treeNode *child) {
    parent->num_children++;
    parent->children = (t_treeNode **)realloc(parent->children, parent->num_children * sizeof(t_treeNode *));
    if (parent->children == NULL) {
        fprintf(stderr, "Error: Memory allocation failed while adding child to tree node\n");
        exit(1);
    }
    parent->children[parent->num_children - 1] = child;
}

void buildTree(t_treeNode *node, int depth, int max_depth, t_move *available_moves, int num_moves, t_map map) {
    if (depth >= max_depth) return;
    for (int i = 0; i < num_moves; i++) {
        t_move current_move = available_moves[i];
        t_localisation new_loc = performMove(node->loc, current_move);

        // Vérifie si la nouvelle localisation est valide
        if (!isValidLocalisation(new_loc.pos, map.x_max, map.y_max)) continue;

        // Vérifie si la cellule est accessible (évite les crevasses)
        if (map.soils[new_loc.pos.y][new_loc.pos.x] == CREVASSE) continue;

        // Calcule le coût cumulé
        int move_cost = _soil_cost[map.soils[new_loc.pos.y][new_loc.pos.x]];
        int new_cost = node->cost + move_cost;

        // Crée un nouvel enfant
        t_treeNode *child = createTreeNode(new_loc, new_cost, current_move, node);
        addChild(node, child);

        // Affiche les détails du nœud
        printf("Depth %d - Move %s to (%d, %d), Orientation %d, Cumulative Cost %d\n",
               depth + 1, getMoveAsString(current_move), new_loc.pos.x, new_loc.pos.y, new_loc.ori, new_cost);

        // Récursion pour construire l'arbre
        buildTree(child, depth + 1, max_depth, available_moves, num_moves, map);
    }
}

// Trouve la feuille avec le coût minimal
t_treeNode* findMinCostLeaf(t_treeNode *node, t_treeNode *min_node) {
    if (node->num_children == 0) {
        printf("Leaf reached: Cumulative Cost %d at position (%d, %d)\n", node->cost, node->loc.pos.x, node->loc.pos.y);
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

// Reconstruit le chemin depuis la feuille jusqu'à la racine
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
        path[i] = pop(&stack);
    }
    free(stack.values);
}

// Libère la mémoire allouée à l'arbre
void freeTree(t_treeNode *root) {
    if (root == NULL) return;
    for (int i = 0; i < root->num_children; i++) {
        freeTree(root->children[i]);
    }
    free(root->children);
    free(root);
}
