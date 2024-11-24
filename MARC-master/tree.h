#ifndef TREE_H
#define TREE_H

#include "loc.h"
#include "moves.h"
#include "map.h"

/**
 * @struct s_treeNode
 * @brief Structure d'un nœud de l'arbre.
 */
typedef struct s_treeNode {
    t_localisation loc;           // Localisation du rover à ce nœud.
    int cost;                     // Coût cumulé pour atteindre ce nœud.
    t_move move;                  // Mouvement effectué pour atteindre ce nœud.
    struct s_treeNode **children; // Tableau des enfants (nœuds suivants).
    int num_children;             // Nombre d'enfants de ce nœud.
    struct s_treeNode *parent;    // Pointeur vers le nœud parent.
} t_treeNode;

// Crée un nœud de l'arbre avec les paramètres spécifiés.
t_treeNode* createTreeNode(t_localisation loc, int cost, t_move move, t_treeNode *parent);

// Ajoute un enfant à un nœud parent.
void addChild(t_treeNode *parent, t_treeNode *child);

// Construit l'arbre des déplacements jusqu'à une profondeur donnée en utilisant les mouvements disponibles.
void buildTree(t_treeNode *node, int depth, int max_depth, t_move *available_moves, int num_moves, t_map map);

// Trouve la feuille avec le coût minimal dans l'arbre.
t_treeNode* findMinCostLeaf(t_treeNode *node, t_treeNode *min_node);

// Reconstruit le chemin depuis la feuille jusqu'à la racine en stockant les mouvements dans path et la longueur dans path_length.
void getPathFromLeaf(t_treeNode *leaf, t_move *path, int *path_length);

// Libère la mémoire allouée à l'arbre à partir de la racine.
void freeTree(t_treeNode *root);

#endif // TREE_H
