//
// Created by samya on 09/11/2024.
//

#ifndef UNTITLED1_TREE_H
#define UNTITLED1_TREE_H


#include "loc.h"
#include "moves.h"
#include "map.h"


typedef struct s_treeNode {
    t_localisation loc;
    int cost;
    t_move move;
    struct s_treeNode **children;
    int num_children;
    struct s_treeNode *parent;
} t_treeNode;

t_treeNode* createTreeNode(t_localisation loc, int cost, t_move move, t_treeNode *parent);


void addChild(t_treeNode *parent, t_treeNode *child);

void freeTree(t_treeNode *root);

void buildTree(t_treeNode *node, int depth, int max_depth, t_move *available_moves, int num_moves, t_map map);

t_treeNode* findMinCostLeaf(t_treeNode *node, t_treeNode *min_node);

void getPathFromLeaf(t_treeNode *leaf, t_move *path, int *path_length);


#endif //UNTITLED1_TREE_H
