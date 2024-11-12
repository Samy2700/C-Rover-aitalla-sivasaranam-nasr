#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "tree.h"

#define MAX_PHASES 10
#define MAX_PATH_LENGTH 10

int main() {
    srand(time(NULL));
    int choice;
    printf("Choose the map to load:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);

    t_map map;
    if (choice == 1) {
        map = createMapFromFile("..\\maps\\example1.map");
    } else if (choice == 2) {
        map = createTrainingMap();
    } else {
        printf("Invalid choice. Loading example1.map by default.\n");
        map = createMapFromFile("..\\maps\\example1.map");
    }

    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);
    printf("Displaying soil types:\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]);
        }
        printf("\n");
    }

    printf("Displaying costs:\n");
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%-5d ", map.costs[i][j]);
        }
        printf("\n");
    }

    // Initialize MARC's location at the base station
    t_position base_pos = getBaseStationPosition(map);
    t_localisation marc_loc = loc_init(base_pos.x, base_pos.y, NORTH);
    printf("MARC initial position: (%d, %d), orientation: %d\n", marc_loc.pos.x, marc_loc.pos.y, marc_loc.ori);
    displayMap(map, marc_loc);

    // Define all possible moves
    t_move all_moves[] = {F_10, F_20, F_30, B_10, T_LEFT, T_RIGHT, U_TURN};
    int total_moves = sizeof(all_moves) / sizeof(all_moves[0]);

    int max_phases = MAX_PHASES;
    int current_phase = 0;
    int mission_completed = 0;

    while (current_phase < max_phases && !mission_completed) {
        printf("\n--- Phase %d ---\n", current_phase + 1);

        // Randomly select 3 unique available moves for this phase
        int num_available_moves = 3;
        t_move available_moves[3];
        for (int i = 0; i < num_available_moves; i++) {
            available_moves[i] = all_moves[rand() % total_moves];
            // Ensure uniqueness
            for (int j = 0; j < i; j++) {
                if (available_moves[j] == available_moves[i]) {
                    available_moves[i] = all_moves[rand() % total_moves];
                    j = -1; // Restart checking
                }
            }
        }
        printf("Available moves for this phase:\n");
        for (int i = 0; i < num_available_moves; i++) {
            printf("- %s\n", getMoveAsString(available_moves[i]));
        }

        // Build the movement tree
        int max_depth = 3; // Depth of 3 for simplicity
        t_treeNode *root = createTreeNode(marc_loc, 0, -1, NULL);
        buildTree(root, 0, max_depth, available_moves, num_available_moves, map, 0, 1);

        // Find the leaf with the minimum cumulative cost
        t_treeNode *min_leaf = findMinCostLeaf(root, NULL);
        if (min_leaf != NULL) {
            t_move path[MAX_PATH_LENGTH];
            int path_length = 0;
            getPathFromLeaf(min_leaf, path, &path_length);
            printf("\nOptimal path found with a total cost of %d:\n", min_leaf->cost);
            for (int i = 0; i < path_length; i++) {
                printf("Step %d: Move %s\n", i + 1, getMoveAsString(path[i]));
            }

            // Apply the movements
            for (int i = 0; i < path_length; i++) {
                printf("\nBefore move %d:\n", i + 1);
                displayMap(map, marc_loc);
                printf("Move %d: %s\n", i + 1, getMoveAsString(path[i]));
                updateLocalisation(&marc_loc, path[i]);

                // Check if MARC is still within the map
                if (!isValidLocalisation(marc_loc.pos, map.x_max, map.y_max)) {
                    printf("MARC has moved out of the map boundaries to position (%d, %d)\n", marc_loc.pos.x, marc_loc.pos.y);
                    mission_completed = 1;
                    break;
                }

                printf("After move %d:\n", i + 1);
                displayMap(map, marc_loc);
                t_soil current_soil = map.soils[marc_loc.pos.y][marc_loc.pos.x];
                printf("MARC is now on soil type %d\n", current_soil);

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

                if (mission_completed) break;

                int case_cost = map.costs[marc_loc.pos.y][marc_loc.pos.x];
                printf("Current cell cost: %d\n", case_cost);
            }
        } else {
            printf("No valid path found for this phase.\n");
        }

        freeTree(root);
        current_phase++;
    }

    if (!mission_completed) {
        printf("\nMaximum number of phases reached without completing the mission.\n");
    }

    // Free allocated memory for the map
    for (int i = 0; i < map.y_max; i++) {
        free(map.soils[i]);
        free(map.costs[i]);
    }
    free(map.soils);
    free(map.costs);

    return 0;
}
