#ifndef TREE_H
#define TREE_H

#include "loc.h"
#include "moves.h"
#include "map.h"

// Structure d'un nœud de l'arbre
typedef struct s_treeNode {
    t_localisation loc;
    int cost;
    t_move move;
    struct s_treeNode **children;
    int num_children;
    struct s_treeNode *parent;
} t_treeNode;

// Crée un nœud de l'arbre
t_treeNode* createTreeNode(t_localisation loc, int cost, t_move move, t_treeNode *parent);

// Ajoute un enfant à un nœud parent
void addChild(t_treeNode *parent, t_treeNode *child);

// Construit l'arbre des déplacements jusqu'à une profondeur donnée
void buildTree(t_treeNode *node, int depth, int max_depth, t_move *available_moves, int num_moves, t_map map);

// Trouve la feuille avec le coût minimal
t_treeNode* findMinCostLeaf(t_treeNode *node, t_treeNode *min_node);

// Reconstruit le chemin depuis la feuille jusqu'à la racine
void getPathFromLeaf(t_treeNode *leaf, t_move *path, int *path_length);

// Libère la mémoire allouée à l'arbre
void freeTree(t_treeNode *root);

#endif // TREE_H
