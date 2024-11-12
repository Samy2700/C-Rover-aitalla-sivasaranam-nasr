#include <stdio.h>
#include "map.h"      
#include "loc.h"      
#include "moves.h"    
#include "tree.h"     


int main() {
    int choice; // Variable pour stocker le choix de la carte
    // Affichage des options de carte à charger
    printf("Choisissez la carte a charger:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("Entrez votre choix (1 ou 2): ");
    scanf("%d", &choice); // Lecture du choix de l'utilisateur

    t_map map; 
    // Chargement de la carte en fonction du choix de l'utilisateur
    if (choice == 1) {
        map = createMapFromFile("..\\maps\\example1.map"); // Chargement de la carte example1.map
    } else if (choice == 2) {
        map = createTrainingMap(); // Chargement de la carte d'entraînement
    } else {
        printf("Choix invalide. Chargement de example1.map par défaut.\n"); // Choix invalide
        map = createMapFromFile("..\\maps\\example1.map"); // Chargement par défaut de la carte example1.map
    }

    // Affichage des dimensions de la carte
    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);

    // Affichage des sols de la carte (structure map.soils)
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]); // Affichage de chaque valeur de sol
        }
        printf("\n");
    }

    // Affichage des coûts de chaque case de la carte (structure map.costs)
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%-5d ", map.costs[i][j]); // Affichage du coût de chaque case
        }
        printf("\n");
    }

    // Initialisation de la localisation de MARC (Position et orientation initiales)
    t_localisation marc_loc = loc_init(0, 0, NORTH); // Position initiale en haut à gauche, orientation vers le nord
    printf("MARC initial position: (%d, %d), orientation: %d\n", marc_loc.pos.x, marc_loc.pos.y, marc_loc.ori); // Affichage de la position et de l'orientation de MARC

    // Affichage de la carte avec la localisation actuelle de MARC
    displayMap(map, marc_loc);

    // Définition des mouvements disponibles pour MARC
    t_move available_moves[] = {F_10, T_LEFT, T_RIGHT, F_20, B_10}; // Liste des mouvements disponibles
    int num_available_moves = sizeof(available_moves) / sizeof(available_moves[0]); // Calcul du nombre de mouvements

    // Affichage des mouvements disponibles
    printf("\nMouvements disponibles pour cette phase :\n");
    for (int i = 0; i < num_available_moves; i++) {
        printf("- %s\n", getMoveAsString(available_moves[i])); // Affichage de chaque mouvement disponible
    }

    int max_depth = 3; // Profondeur maximale de l'arbre de recherche
    t_treeNode *root = createTreeNode(marc_loc, 0, -1, NULL); // Création du noeud racine de l'arbre avec la position initiale de MARC
    buildTree(root, 0, max_depth, available_moves, num_available_moves, map, 0, 1); // Construction de l'arbre de recherche

    // Recherche du chemin optimal dans l'arbre
    t_treeNode *min_leaf = findMinCostLeaf(root, NULL); // Trouver le feuille ayant le coût minimum

    // Si un chemin valide est trouvé, on l'affiche et on applique les mouvements
    if (min_leaf != NULL) {
        t_move path[10]; // Tableau pour stocker les mouvements du chemin optimal
        int path_length = 0; // Longueur du chemin
        getPathFromLeaf(min_leaf, path, &path_length); // Récupération du chemin depuis la feuille jusqu'à la racine de l'arbre

        // Affichage du chemin optimal et de son coût
        printf("\nChemin optimal trouvé avec un coût total de %d :\n", min_leaf->cost);
        t_treeNode *current_node = min_leaf;
        int step = path_length;
        t_localisation locs[10]; // Tableau des localisations
        int costs[10]; // Tableau des coûts
        while (current_node->parent != NULL) {
            locs[step - 1] = current_node->loc; // Récupération de la localisation
            costs[step - 1] = current_node->cost; // Récupération du coût
            current_node = current_node->parent; // Remontée vers le parent dans l'arbre
            step--;
        }

        // Affichage de chaque étape du chemin optimal
        for (int i = 0; i < path_length; i++) {
            printf("Étape %d: Mouvement %s, Position (%d, %d), Orientation %d, Coût cumulé %d\n",
                   i + 1, getMoveAsString(path[i]), locs[i].pos.x, locs[i].pos.y, locs[i].ori, costs[i]);
        }

        // Application des mouvements sur la carte
        printf("\nApplication des mouvements :\n");
        marc_loc = loc_init(0, 0, NORTH); // Réinitialisation de la localisation de MARC
        for (int i = 0; i < path_length; i++) {
            printf("\nAvant le mouvement %d :\n", i + 1);
            displayMap(map, marc_loc); // Affichage de la carte avant le mouvement
            printf("Mouvement %d : %s\n", i + 1, getMoveAsString(path[i])); // Affichage du mouvement à appliquer
            updateLocalisation(&marc_loc, path[i]); // Mise à jour de la localisation de MARC

            // Vérification si MARC est sorti de la carte
            if (!isValidLocalisation(marc_loc.pos, map.x_max, map.y_max)) {
                printf("MARC est sorti de la carte à la position (%d, %d)\n", marc_loc.pos.x, marc_loc.pos.y);
                break;
            }

            // Affichage de la carte après le mouvement
            printf("Après le mouvement %d :\n", i + 1);
            displayMap(map, marc_loc);

            // Affichage du type de sol sur lequel MARC se trouve
            t_soil current_soil = map.soils[marc_loc.pos.y][marc_loc.pos.x];
            printf("MARC se trouve sur le sol de type %d\n", current_soil);

            // Vérification du type de sol et affichage des messages appropriés
            switch (current_soil) {
                case ERG:
                    printf("Attention : Sol Erg, le prochain mouvement sera affecté.\n");
                    break;
                case REG:
                    printf("Attention : Sol Reg, seulement 4 mouvements seront disponibles lors de la prochaine phase.\n");
                    break;
                case CREVASSE:
                    printf("Danger : MARC est tombé dans une crevasse ! Fin de la mission.\n");
                    return 1; // Fin de la mission si MARC tombe dans une crevasse
                default:
                    break;
            }

            // Affichage du coût de la case actuelle
            int case_cost = map.costs[marc_loc.pos.y][marc_loc.pos.x];
            printf("Coût de la case actuelle : %d\n", case_cost);
        }
    } else {
        // Si aucun chemin valide n'est trouvé
        printf("Aucun chemin valide trouvé.\n");
    }

    freeTree(root); // Libération de la mémoire allouée pour l'arbre
    return 0; // Fin du programme
}
