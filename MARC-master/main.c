#include <stdio.h>
#include "map.h"
#include "tree.h"

int main() {
    // Définition des mouvements disponibles pour cette phase
    t_move available_moves[] = {F_10, T_LEFT, T_RIGHT, F_20, B_10};
    int num_available_moves = 5;
    // Initialiser la localisation de MARC
    t_localisation marc_loc = loc_init(0, 0, NORTH);

    t_map map = createMapFromFile("..\\maps\\example1.map");
    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);
    for (int i = 0; i < map.y_max; i++)
    {
        for (int j = 0; j < map.x_max; j++)
        {
            printf("%d ", map.soils[i][j]);
        }
        printf("\n");
    }
    // printf the costs, aligned left 5 digits
    for (int i = 0; i < map.y_max; i++)
    {
        for (int j = 0; j < map.x_max; j++)
        {
            printf("%-5d ", map.costs[i][j]);
        }
        printf("\n");
    }
    displayMap(map);

    // Construire l'arbre avec 3 choix parmi 5 mouvements
    int max_depth = 3;
    t_treeNode *root = createTreeNode(marc_loc, 0, -1, NULL); // -1 pour le mouvement initial inexistant
    buildTree(root, 0, max_depth, available_moves, num_available_moves, map);

// Trouver la feuille avec le coût minimal
    t_treeNode *min_leaf = findMinCostLeaf(root, NULL);

    if (min_leaf != NULL) {
        // Retracer le chemin depuis la feuille jusqu'à la racine
        t_move path[10]; // Taille maximale du chemin
        int path_length = 0;
        getPathFromLeaf(min_leaf, path, &path_length);

        // Afficher le chemin
        printf("Optimal path with cost %d:\n", min_leaf->cost);
        for (int i = 0; i < path_length; i++) {
            printf("%s -> ", getMoveAsString(path[i]));
        }
        printf("END\n");
    } else {
        printf("No valid path found.\n");
    }

// Libérer la mémoire de l'arbre
    freeTree(root);
    return 0;
}
