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
    t_localisation loc;           /**< Localisation du rover à ce nœud. */
    int cost;                     /**< Coût cumulé pour atteindre ce nœud. */
    t_move move;                  /**< Mouvement effectué pour atteindre ce nœud. */
    struct s_treeNode **children; /**< Tableau des enfants (nœuds suivants). */
    int num_children;             /**< Nombre d'enfants de ce nœud. */
    struct s_treeNode *parent;    /**< Pointeur vers le nœud parent. */
} t_treeNode;

/**
 * @brief Crée un nœud de l'arbre avec les paramètres spécifiés.
 * @param loc Localisation du rover à ce nœud.
 * @param cost Coût cumulé pour atteindre ce nœud.
 * @param move Mouvement effectué pour atteindre ce nœud.
 * @param parent Pointeur vers le nœud parent.
 * @return Le nœud créé.
 */
t_treeNode* createTreeNode(t_localisation loc, int cost, t_move move, t_treeNode *parent);

/**
 * @brief Ajoute un enfant à un nœud parent.
 * @param parent Pointeur vers le nœud parent.
 * @param child Pointeur vers l'enfant à ajouter.
 */
void addChild(t_treeNode *parent, t_treeNode *child);

/**
 * @brief Construit l'arbre des déplacements jusqu'à une profondeur donnée en utilisant les mouvements disponibles.
 * @param node Pointeur vers le nœud actuel.
 * @param depth Profondeur actuelle.
 * @param max_depth Profondeur maximale autorisée.
 * @param available_moves Tableau des mouvements disponibles.
 * @param num_moves Nombre de mouvements disponibles.
 * @param map La carte.
 */
void buildTree(t_treeNode *node, int depth, int max_depth, t_move *available_moves, int num_moves, t_map map);

/**
 * @brief Trouve la feuille avec le coût minimal dans l'arbre.
 * @param node Pointeur vers le nœud actuel.
 * @param min_node Pointeur vers le nœud feuille avec le coût minimal actuel.
 * @return Pointeur vers le nœud feuille avec le coût minimal.
 */
t_treeNode* findMinCostLeaf(t_treeNode *node, t_treeNode *min_node);

/**
 * @brief Reconstruit le chemin depuis la feuille jusqu'à la racine en stockant les mouvements dans path et la longueur dans path_length.
 * @param leaf Pointeur vers la feuille de départ.
 * @param path Tableau pour stocker le chemin des mouvements.
 * @param path_length Pointeur vers la variable stockant la longueur du chemin.
 */
void getPathFromLeaf(t_treeNode *leaf, t_move *path, int *path_length);

/**
 * @brief Libère la mémoire allouée à l'arbre à partir de la racine.
 * @param root Pointeur vers la racine de l'arbre.
 */
void freeTree(t_treeNode *root);

#endif // TREE_H
