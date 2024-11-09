#include <stdio.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "tree.h"

int main() {
    int choice;
    printf("Choisissez la carte à charger:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("Entrez votre choix (1 ou 2): ");
    scanf("%d", &choice);
    t_map map;
    if (choice == 1) {
        map = createMapFromFile("..\\maps\\example1.map");
    } else if (choice == 2) {
        map = createTrainingMap();
    } else {
        printf("Choix invalide. Chargement de example1.map par défaut.\n");
        map = createMapFromFile("..\\maps\\example1.map");
    }
    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]);
        }
        printf("\n");
    }
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%-5d ", map.costs[i][j]);
        }
        printf("\n");
    }
    t_localisation marc_loc = loc_init(0, 0, NORTH);
    printf("MARC initial position: (%d, %d), orientation: %d\n", marc_loc.pos.x, marc_loc.pos.y, marc_loc.ori);
    displayMap(map, marc_loc);
    t_move available_moves[] = {F_10, T_LEFT, T_RIGHT, F_20, B_10};
    int num_available_moves = sizeof(available_moves) / sizeof(available_moves[0]);
    printf("\nMouvements disponibles pour cette phase :\n");
    for (int i = 0; i < num_available_moves; i++) {
        printf("- %s\n", getMoveAsString(available_moves[i]));
    }
    int max_depth = 3;
    t_treeNode *root = createTreeNode(marc_loc, 0, -1, NULL);
    buildTree(root, 0, max_depth, available_moves, num_available_moves, map, 0, 1);
    t_treeNode *min_leaf = findMinCostLeaf(root, NULL);
    if (min_leaf != NULL) {
        t_move path[10];
        int path_length = 0;
        getPathFromLeaf(min_leaf, path, &path_length);
        printf("\nChemin optimal trouvé avec un coût total de %d :\n", min_leaf->cost);
        t_treeNode *current_node = min_leaf;
        int step = path_length;
        t_localisation locs[10];
        int costs[10];
        while (current_node->parent != NULL) {
            locs[step - 1] = current_node->loc;
            costs[step - 1] = current_node->cost;
            current_node = current_node->parent;
            step--;
        }
        for (int i = 0; i < path_length; i++) {
            printf("Étape %d: Mouvement %s, Position (%d, %d), Orientation %d, Coût cumulé %d\n",
                   i + 1, getMoveAsString(path[i]), locs[i].pos.x, locs[i].pos.y, locs[i].ori, costs[i]);
        }
        printf("\nApplication des mouvements :\n");
        marc_loc = loc_init(0, 0, NORTH);
        for (int i = 0; i < path_length; i++) {
            printf("\nAvant le mouvement %d :\n", i + 1);
            displayMap(map, marc_loc);
            printf("Mouvement %d : %s\n", i + 1, getMoveAsString(path[i]));
            updateLocalisation(&marc_loc, path[i]);
            if (!isValidLocalisation(marc_loc.pos, map.x_max, map.y_max)) {
                printf("MARC est sorti de la carte à la position (%d, %d)\n", marc_loc.pos.x, marc_loc.pos.y);
                break;
            }
            printf("Après le mouvement %d :\n", i + 1);
            displayMap(map, marc_loc);
            t_soil current_soil = map.soils[marc_loc.pos.y][marc_loc.pos.x];
            printf("MARC se trouve sur le sol de type %d\n", current_soil);
            switch (current_soil) {
                case ERG:
                    printf("Attention : Sol Erg, le prochain mouvement sera affecté.\n");
                    break;
                case REG:
                    printf("Attention : Sol Reg, seulement 4 mouvements seront disponibles lors de la prochaine phase.\n");
                    break;
                case CREVASSE:
                    printf("Danger : MARC est tombé dans une crevasse ! Fin de la mission.\n");
                    return 1;
                default:
                    break;
            }
            int case_cost = map.costs[marc_loc.pos.y][marc_loc.pos.x];
            printf("Coût de la case actuelle : %d\n", case_cost);
        }
    } else {
        printf("Aucun chemin valide trouvé.\n");
    }
    freeTree(root);
    return 0;
}
