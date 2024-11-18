#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "tree.h"

#define MAX_PHASES 10
#define MAX_DEPTH 5
#define MAX_PATH_LENGTH 20

int main() {
    srand(time(NULL));
    int choice;
    printf("Choose the map to load:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("3. custom1.map\n"); // Ajouter des options pour les cartes personnalisées
    printf("4. custom2.map\n");
    printf("Enter your choice (1, 2, 3, or 4): ");
    scanf("%d", &choice);

    t_map map;
#if defined(_WIN32) || defined(_WIN64)
    if (choice == 1) {
        map = createMapFromFile("..\\maps\\example1.map");
    } else if (choice == 2) {
        map = createTrainingMap();
    } else if (choice == 3) {
        map = createMapFromFile("..\\maps\\custom1.map"); // Exemple de carte personnalisée
    } else if (choice == 4) {
        map = createMapFromFile("..\\maps\\custom2.map"); // Exemple de carte personnalisée
    } else {
        printf("Invalid choice. Loading example1.map by default.\n");
        map = createMapFromFile("..\\maps\\example1.map");
    }
#else
    if (choice == 1) {
        map = createMapFromFile("../maps/example1.map");
    } else if (choice == 2) {
        map = createTrainingMap();
    } else if (choice == 3) {
        map = createMapFromFile("../maps/custom1.map"); // Exemple de carte personnalisée
    } else if (choice == 4) {
        map = createMapFromFile("../maps/custom2.map"); // Exemple de carte personnalisée
    } else {
        printf("Invalid choice. Loading example1.map by default.\n");
        map = createMapFromFile("../maps/example1.map");
    }
#endif

    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);
    printf("Displaying soil types and costs:\n");

    // Affichage des types de sols
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]);
        }
        printf("\n");
    }

    // Affichage des coûts, alignés à gauche sur 5 caractères
    printf("\nDisplaying costs:\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%-5d ", map.costs[i][j]);
        }
        printf("\n");
    }

    // Trouvons une position de départ qui n'est ni la station de base ni une crevasse
    t_position base_pos = getBaseStationPosition(map);
    t_localisation marc_loc;
    int start_found = 0;
    int start_cost = 0;
    for (int i = 0; i < map.y_max && !start_found; i++) {
        for (int j = 0; j < map.x_max && !start_found; j++) {
            if (map.soils[i][j] != BASE_STATION && map.soils[i][j] != CREVASSE) {
                marc_loc = loc_init(j, i, NORTH);
                start_cost = map.costs[i][j]; // Coût de la cellule de départ
                start_found = 1;
            }
        }
    }

    if (!start_found) {
        fprintf(stderr, "Error: No valid starting position found (no cell is non-base and non-crevasse)\n");
        // Libération de la mémoire allouée pour la carte
        for (int i = 0; i < map.y_max; i++) {
            free(map.soils[i]);
            free(map.costs[i]);
        }
        free(map.soils);
        free(map.costs);
        exit(1);
    }

    printf("\nMARC initial position: (%d, %d), orientation: %d (NORTH=0, EAST=1, SOUTH=2, WEST=3)\n", marc_loc.pos.x, marc_loc.pos.y, marc_loc.ori);
    displayMap(map, marc_loc);

    int current_phase = 0;
    int mission_completed = 0;

    while (current_phase < MAX_PHASES && !mission_completed) {
        printf("\n--- Phase %d ---\n", current_phase + 1);

        // Sélection aléatoire de 5 mouvements parmi les 9 possibles
        int num_available_moves = 5;
        t_move available_moves[5];
        for (int i = 0; i < num_available_moves; i++) {
            available_moves[i] = rand() % 9;
            // Assurer l'unicité des mouvements
            for (int j = 0; j < i; j++) {
                if (available_moves[j] == available_moves[i]) {
                    available_moves[i] = rand() % 9;
                    j = -1; // Recommencer la vérification
                }
            }
        }

        printf("Available moves for this phase:\n");
        for (int i = 0; i < num_available_moves; i++) {
            printf("%d. %s\n", i + 1, getMoveAsString(available_moves[i]));
        }

        // Mesure du temps de construction de l'arbre
        clock_t start_build = clock();
        t_treeNode *root = createTreeNode(marc_loc, start_cost, -1, NULL);
        buildTree(root, 0, MAX_DEPTH, available_moves, num_available_moves, map);
        clock_t end_build = clock();
        double time_build = ((double)(end_build - start_build)) / CLOCKS_PER_SEC;

        // Mesure du temps de recherche de la feuille minimale
        clock_t start_search = clock();
        t_treeNode *min_leaf = findMinCostLeaf(root, NULL);
        clock_t end_search = clock();
        double time_search = ((double)(end_search - start_search)) / CLOCKS_PER_SEC;

        // Mesure du temps de reconstruction du chemin
        clock_t start_path = clock();
        t_move path[MAX_PATH_LENGTH];
        int path_length = 0;
        getPathFromLeaf(min_leaf, path, &path_length);
        clock_t end_path = clock();
        double time_path = ((double)(end_path - start_path)) / CLOCKS_PER_SEC;

        printf("\n--- Performance Metrics ---\n");
        printf("Tree Construction Time: %.6f seconds\n", time_build);
        printf("Leaf Search Time: %.6f seconds\n", time_search);
        printf("Path Calculation Time: %.6f seconds\n", time_path);
        printf("---------------------------\n");

        // Affichage du chemin optimal
        printf("\nOptimal path found with a total cost of %d:\n", min_leaf->cost);
        for (int i = 0; i < path_length; i++) {
            printf("Step %d: Move %s\n", i + 1, getMoveAsString(path[i]));
        }

        // Application des mouvements du chemin optimal
        for (int i = 0; i < path_length; i++) {
            printf("\nBefore move %d:\n", i + 1);
            displayMap(map, marc_loc);
            printf("Move %d: %s\n", i + 1, getMoveAsString(path[i]));
            updateLocalisation(&marc_loc, path[i]);

            // Vérifier si MARC est sorti de la carte
            if (!isValidLocalisation(marc_loc.pos, map.x_max, map.y_max)) {
                printf("MARC has moved out of the map boundaries to position (%d, %d)\n", marc_loc.pos.x, marc_loc.pos.y);
                printf("Mission failed.\n");
                mission_completed = 1;
                break;
            }

            // Affichage de la carte après le mouvement
            printf("\nAfter move %d:\n", i + 1);
            displayMap(map, marc_loc);

            // Récupération du type de sol actuel
            t_soil current_soil = map.soils[marc_loc.pos.y][marc_loc.pos.x];
            printf("MARC is now on soil type %d\n", current_soil);

            // Gestion des conditions de sol
            switch (current_soil) {
                case ERG:
                    printf("Warning: Erg soil, next movement will be affected.\n");
                    break;
                case REG:
                    printf("Warning: Reg soil, only 4 movements will be available in the next phase.\n");
                    break;
                case CREVASSE:
                    printf("Danger: MARC has fallen into a crevasse! Mission failed.\n");
                    mission_completed = 1;
                    break;
                case BASE_STATION:
                    printf("Congratulations: MARC has arrived at the base! Mission accomplished.\n");
                    mission_completed = 1;
                    break;
                default:
                    break;
            }

            // Affichage du coût de la cellule actuelle
            int case_cost = map.costs[marc_loc.pos.y][marc_loc.pos.x];
            printf("Current cell cost: %d\n", case_cost);

            if (mission_completed) break;
        }

        // Libération de la mémoire allouée à l'arbre
        freeTree(root);
        current_phase++;

        // Mise à jour du coût de départ pour la prochaine phase
        start_cost = map.costs[marc_loc.pos.y][marc_loc.pos.x];
    }

    if (!mission_completed) {
        printf("\nMaximum number of phases reached without completing the mission.\n");
    }

    // Libération de la mémoire allouée pour la carte
    for (int i = 0; i < map.y_max; i++) {
        free(map.soils[i]);
        free(map.costs[i]);
    }
    free(map.soils);
    free(map.costs);

    return 0;
}
