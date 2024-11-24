#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "tree.h"

#define MAX_DEPTH 5
#define MAX_PATH_LENGTH 20
#define MAX_ENERGY 1000 // Énergie initiale du rover

int main() {
    srand(time(NULL));
    int choice;
    printf("Choose the map to load:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("3. custom1.map\n");
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
        map = createMapFromFile("..\\maps\\custom1.map");
    } else if (choice == 4) {
        map = createMapFromFile("..\\maps\\custom2.map");
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
        map = createMapFromFile("../maps/custom1.map");
    } else if (choice == 4) {
        map = createMapFromFile("../maps/custom2.map");
    } else {
        printf("Invalid choice. Loading example1.map by default.\n");
        map = createMapFromFile("../maps/example1.map");
    }
#endif

    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);

    // Affichage des types de sols
    printf("Displaying soil types:\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]);
        }
        printf("\n");
    }

    // Affichage des coûts
    printf("\nDisplaying costs:\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%-5d ", map.costs[i][j]);
        }
        printf("\n");
    }

    // Initialisation de la position du rover
    t_localisation marc_loc;
    int start_found = 0;
    int start_cost = 0;
    for (int i = 0; i < map.y_max && !start_found; i++) {
        for (int j = 0; j < map.x_max && !start_found; j++) {
            if (map.soils[i][j] != BASE_STATION && map.soils[i][j] != CREVASSE) {
                marc_loc = loc_init(j, i, NORTH);
                start_cost = map.costs[i][j];
                start_found = 1;
            }
        }
    }

    if (!start_found) {
        fprintf(stderr, "Error: No valid starting position found.\n");
        // Libération de la mémoire
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

    int mission_completed = 0;
    int current_phase = 0;
    int total_cost = 0;
    int energy = MAX_ENERGY; // Énergie initiale du rover
    int erg_effect = 0;      // Effet du terrain ERG sur le prochain mouvement

    // Définition des coûts des mouvements
    static const int _move_cost[9] = {
            10, // F_10
            20, // F_20
            30, // F_30
            10, // B_10
            20, // B_20
            5,  // T_LEFT
            5,  // T_RIGHT
            10, // U_TURN
            15  // L_90
    };

    while (!mission_completed) {
        printf("\n--- Phase %d ---\n", current_phase + 1);

        // Afficher l'énergie restante
        printf("Energy remaining: %d\n", energy);

        // Demander à l'utilisateur de sélectionner les mouvements disponibles pour la phase suivante
        int num_available_moves = 0;
        printf("Enter the number of available moves for the next phase (up to 9): ");
        scanf("%d", &num_available_moves);

        if (num_available_moves < 1 || num_available_moves > 9) {
            printf("Invalid number of moves. Defaulting to 5 moves.\n");
            num_available_moves = 5;
        }

        printf("Select %d moves from the list below by entering their numbers separated by spaces:\n", num_available_moves);
        for (int i = 0; i < 9; i++) {
            printf("%d. %s\n", i + 1, getMoveAsString(i));
        }

        int selected_moves_indices[9];
        t_move available_moves[9];
        for (int i = 0; i < num_available_moves; i++) {
            scanf("%d", &selected_moves_indices[i]);
            if (selected_moves_indices[i] < 1 || selected_moves_indices[i] > 9) {
                printf("Invalid move number. Please enter a number between 1 and 9.\n");
                i--; // Redemander ce mouvement
            } else {
                available_moves[i] = selected_moves_indices[i] - 1;
            }
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

        // Application du premier mouvement du chemin optimal
        t_move selected_move = path[0];

        printf("\nExecuting move: %s\n", getMoveAsString(selected_move));

        // Récupérer le coût du mouvement
        int move_cost = _move_cost[selected_move];

        // Appliquer l'effet du terrain ERG
        if (erg_effect) {
            move_cost *= 2;
            erg_effect = 0; // Réinitialiser l'effet après l'avoir appliqué
        }

        // Mettre à jour la localisation de MARC
        updateLocalisation(&marc_loc, selected_move);

        // Vérifier si MARC est sorti de la carte
        int out_of_bounds = 0;
        if (!isValidLocalisation(marc_loc.pos, map.x_max, map.y_max)) {
            printf("MARC has moved out of the map boundaries to position (%d, %d)\n", marc_loc.pos.x, marc_loc.pos.y);
            out_of_bounds = 1;
        }

        // Afficher la carte mise à jour
        displayMap(map, marc_loc);

        if (!out_of_bounds) {
            // Récupérer le type de sol actuel
            t_soil current_soil = map.soils[marc_loc.pos.y][marc_loc.pos.x];

            // Afficher le type de sol
            printf("MARC is now on soil type %d\n", current_soil);

            // Gestion des conditions de sol
            switch (current_soil) {
                case ERG:
                    printf("Warning: Erg soil, next movement will cost double.\n");
                    erg_effect = 1;
                    break;
                case REG:
                    printf("Warning: Reg soil, movement options are limited in the next phase.\n");
                    // Limiter les mouvements disponibles (par exemple, seulement les rotations)
                    // Vous pouvez implémenter cela si nécessaire
                    break;
                case PENTE:
                    printf("Notice: Slope terrain, movement costs are increased.\n");
                    move_cost += 5; // Augmenter le coût du mouvement
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

            // Affichage du coût du mouvement
            printf("Movement cost: %d\n", move_cost);

            // Décrémenter l'énergie
            energy -= move_cost;

            // Vérifier si l'énergie est épuisée
            if (energy <= 0) {
                printf("MARC has run out of energy! Mission failed.\n");
                mission_completed = 1;
            } else {
                // Afficher l'énergie restante
                printf("Energy remaining: %d\n", energy);
            }

            // Mise à jour du coût de départ pour la prochaine phase
            start_cost = map.costs[marc_loc.pos.y][marc_loc.pos.x];

            if (mission_completed) {
                printf("Total energy used: %d\n", MAX_ENERGY - energy);
            }
        } else {
            printf("MARC is out of bounds.\n");
            mission_completed = 1;
            printf("Total energy used before failure: %d\n", MAX_ENERGY - energy);
        }

        // Libération de la mémoire allouée à l'arbre
        freeTree(root);

        // Vérifier si la mission est terminée
        if (mission_completed) break;

        // Incrémenter la phase
        current_phase++;
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
