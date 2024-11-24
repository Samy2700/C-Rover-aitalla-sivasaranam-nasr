#include <stdlib.h>
#include <stdio.h>
#include "tree.h"
#include "stack.h"

/**
 * @brief Cree un noeud de l'arbre avec les parametres specifiques.
 * @param loc Localisation du rover a ce noeud.
 * @param cost Cout cumule pour atteindre ce noeud.
 * @param move Mouvement effectue pour atteindre ce noeud.
 * @param parent Pointeur vers le noeud parent.
 * @return Le noeud cree.
 */
t_treeNode* createTreeNode(t_localisation loc, int cost, t_move move, t_treeNode *parent) {
    // Allocation memoire pour le noeud
    t_treeNode *node = (t_treeNode *)malloc(sizeof(t_treeNode));
    if (node == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for tree node\n");
        exit(1); // Terminer le programme en cas d'echec d'allocation.
    }

    node->loc = loc;             // Assignation de la localisation.
    node->cost = cost;           // Assignation du cout cumule.
    node->move = move;           // Assignation du mouvement effectue.
    node->children = NULL;       // Initialisation du tableau des enfants a NULL.
    node->num_children = 0;      // Initialisation du nombre d'enfants a 0.
    node->parent = parent;       // Assignation du noeud parent.

    return node; // Retourne le noeud cree.
}

/**
 * @brief Ajoute un enfant a un noeud parent.
 * @param parent Pointeur vers le noeud parent.
 * @param child Pointeur vers l'enfant a ajouter.
 */
void addChild(t_treeNode *parent, t_treeNode *child) {
    parent->num_children++; // Incrementation du nombre d'enfants du parent.
    // Reallocation memoire pour ajouter un nouvel enfant
    parent->children = (t_treeNode **)realloc(parent->children, parent->num_children * sizeof(t_treeNode *));
    if (parent->children == NULL) {
        fprintf(stderr, "Error: Memory allocation failed while adding child to tree node\n");
        exit(1); // Terminer le programme en cas d'echec d'allocation.
    }
    parent->children[parent->num_children - 1] = child; // Ajout de l'enfant au tableau.
}

/**
 * @brief Construit l'arbre des deplacements jusqu'a une profondeur donnee en utilisant les mouvements disponibles.
 * @param node Pointeur vers le noeud actuel.
 * @param depth Profondeur actuelle.
 * @param max_depth Profondeur maximale autorisee.
 * @param available_moves Tableau des mouvements disponibles.
 * @param num_moves Nombre de mouvements disponibles.
 * @param map La carte.
 */
void buildTree(t_treeNode *node, int depth, int max_depth, t_move *available_moves, int num_moves, t_map map) {
    if (depth >= max_depth) return; // Si la profondeur maximale est atteinte, arreter la recursion.

    for (int i = 0; i < num_moves; i++) { // Parcours des mouvements disponibles.
        t_move mouvement_courant = available_moves[i];
        t_localisation nouvelle_loc = performMove(node->loc, mouvement_courant); // Obtention de la nouvelle localisation apres le mouvement.

        // Vérifie si la nouvelle localisation est valide (à l'intérieur des limites de la carte)
        if (!isValidLocalisation(nouvelle_loc.pos, map.x_max, map.y_max)) continue;

        // Vérifie si la cellule est accessible (évite les crevasses)
        if (map.soils[nouvelle_loc.pos.y][nouvelle_loc.pos.x] == CREVASSE) continue;

        // Calcule le coût cumulé en ajoutant le coût du sol actuel
        int cout_mouvement = _soil_cost[map.soils[nouvelle_loc.pos.y][nouvelle_loc.pos.x]];
        int nouveau_cout = node->cost + cout_mouvement;

        // Création d'un nouvel enfant avec la nouvelle localisation et le coût cumulé
        t_treeNode *enfant = createTreeNode(nouvelle_loc, nouveau_cout, mouvement_courant, node);
        addChild(node, enfant); // Ajout de l'enfant au noeud parent.

        // Affichage des details du noeud pour le debogage ou l'information
        printf("Profondeur %d - Mouvement %s vers (%d, %d), Orientation %d, Cout cumule %d\n",
               depth + 1, getMoveAsString(mouvement_courant), nouvelle_loc.pos.x, nouvelle_loc.pos.y, nouvelle_loc.ori, nouveau_cout);

        // Recursion pour construire l'arbre avec l'enfant cree
        buildTree(enfant, depth + 1, max_depth, available_moves, num_moves, map);
    }
}

/**
 * @brief Trouve la feuille avec le cout minimal dans l'arbre.
 * @param node Pointeur vers le noeud actuel.
 * @param min_node Pointeur vers le noeud feuille avec le cout minimal actuel.
 * @return Pointeur vers le noeud feuille avec le cout minimal.
 */
t_treeNode* findMinCostLeaf(t_treeNode *node, t_treeNode *min_node) {
    // Si le noeud n'a pas d'enfants, c'est une feuille
    if (node->num_children == 0) {
        printf("Feuille atteinte: Cout cumule %d a la position (%d, %d)\n", node->cost, node->loc.pos.x, node->loc.pos.y);
        // Si c'est la premiere feuille ou si son cout est inferieur au cout minimal actuel, mettre a jour min_node
        if (min_node == NULL || node->cost < min_node->cost) {
            return node;
        }
        return min_node;
    }

    // Parcours recursif des enfants pour trouver la feuille avec le cout minimal
    for (int i = 0; i < node->num_children; i++) {
        min_node = findMinCostLeaf(node->children[i], min_node);
    }
    return min_node; // Retourne la feuille avec le cout minimal.
}

/**
 * @brief Libere la memoire allouee a l'arbre de facon recursive.
 * @param root Pointeur vers la racine de l'arbre.
 */
void freeTree(t_treeNode *root) {
    if (root == NULL) return; // Si le noeud est NULL, rien a faire.

    // Libere recursivement les enfants
    for (int i = 0; i < root->num_children; i++) {
        freeTree(root->children[i]);
    }
    free(root->children); // Libere le tableau des enfants.
    free(root);           // Libere le noeud lui-meme.
}

/**
 * @brief Reconstruit le chemin depuis la feuille jusqu'a la racine en stockant les mouvements dans path et la longueur dans path_length.
 * @param leaf Pointeur vers la feuille de depart.
 * @param path Tableau pour stocker le chemin des mouvements.
 * @param path_length Pointeur vers la variable stockant la longueur du chemin.
 */
void getPathFromLeaf(t_treeNode *leaf, t_move *path, int *path_length) {
    t_stack pile = createStack(100); // Creation d'une pile pour stocker les mouvements.
    t_treeNode *courant = leaf;
    *path_length = 0;

    // Remontee de l'arbre depuis la feuille jusqu'a la racine en empilant les mouvements
    while (courant->parent != NULL) {
        push(&pile, courant->move); // Empile le mouvement effectue pour atteindre ce noeud.
        courant = courant->parent;    // Passe au noeud parent.
        (*path_length)++;            // Incremente la longueur du chemin.
    }

    // Depile les mouvements pour reconstruire le chemin dans l'ordre de la racine vers la feuille
    for (int i = 0; i < *path_length; i++) {
        t_move mouvement_en_pile = top(pile);          // Consulte le mouvement au sommet de la pile sans le retirer.
        path[i] = mouvement_en_pile;                    // Stocke le mouvement dans le tableau de chemin.
        printf("Mouvement a la position %d depuis le sommet: %s\n", i + 1, getMoveAsString(mouvement_en_pile)); // Affiche le mouvement.
        pop(&pile);                                      // Retire le mouvement de la pile.
    }

    free(pile.values); // Libere la memoire allouee a la pile.
}
