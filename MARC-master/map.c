#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "loc.h"
#include "queue.h"
#include "string.h"

/**
 * @brief Obtient la position de la station de base sur la carte.
 * @param map La carte.
 * @return La position de la station de base.
 */
t_position getBaseStationPosition(t_map map) {
    t_position pos;
    int i = 0;
    int trouve = 0;

    // Parcours de la carte pour trouver la station de base
    while (i < map.y_max && !trouve) {
        int j = 0;
        while (j < map.x_max && !trouve) {
            if (map.soils[i][j] == BASE_STATION) {
                pos.x = j; /**< Assignation de la coordonnée x. */
                pos.y = i; /**< Assignation de la coordonnée y. */
                trouve = 1; /**< Indicateur de trouvaille. */
            }
            j++;
        }
        i++;
    }

    // Si la station de base n'est pas trouvée, afficher une erreur et quitter
    if (!trouve) {
        fprintf(stderr, "Error: base station not found in the map\n");
        exit(1);
    }
    return pos; /**< Retourne la position de la station de base. */
}

/**
 * @brief Supprime les fausses crevasses sur la carte.
 * @param map La carte.
 */
void removeFalseCrevasses(t_map map) {
    int termine = 0;
    int imin, jmin;

    while (!termine) {
        int min_cost = COST_UNDEF;
        imin = map.y_max;
        jmin = map.x_max;

        // Recherche de la cellule non crevasse avec le coût minimal
        for (int i = 0; i < map.y_max; i++) {
            for (int j = 0; j < map.x_max; j++) {
                if (map.soils[i][j] != CREVASSE && map.costs[i][j] > 10000 && map.costs[i][j] < min_cost) {
                    min_cost = map.costs[i][j];
                    imin = i;
                    jmin = j;
                }
            }
        }

        // Si une cellule est trouvée, ajuster son coût
        if (imin < map.y_max && jmin < map.x_max) {
            t_position pos;
            pos.x = jmin;
            pos.y = imin;

            t_position gauche = LEFT(pos); /**< Position à gauche. */
            t_position droite = RIGHT(pos); /**< Position à droite. */
            t_position haut = UP(pos);       /**< Position en haut. */
            t_position bas = DOWN(pos);       /**< Position en bas. */

            int min_voisin = COST_UNDEF;

            // Trouver le coût minimal des voisins
            if (isValidLocalisation(gauche, map.x_max, map.y_max)) {
                min_voisin = (map.costs[gauche.y][gauche.x] < min_voisin) ? map.costs[gauche.y][gauche.x] : min_voisin;
            }
            if (isValidLocalisation(droite, map.x_max, map.y_max)) {
                min_voisin = (map.costs[droite.y][droite.x] < min_voisin) ? map.costs[droite.y][droite.x] : min_voisin;
            }
            if (isValidLocalisation(haut, map.x_max, map.y_max)) {
                min_voisin = (map.costs[haut.y][haut.x] < min_voisin) ? map.costs[haut.y][haut.x] : min_voisin;
            }
            if (isValidLocalisation(bas, map.x_max, map.y_max)) {
                min_voisin = (map.costs[bas.y][bas.x] < min_voisin) ? map.costs[bas.y][bas.x] : min_voisin;
            }

            // Calcul du coût de la cellule actuelle
            int cout_self = _soil_cost[map.soils[imin][jmin]];
            // Mise à jour du coût de la cellule avec le coût minimal de ses voisins plus son propre coût
            map.costs[imin][jmin] = (min_voisin + cout_self < map.costs[imin][jmin]) ? min_voisin + cout_self : map.costs[imin][jmin];
        } else {
            termine = 1; /**< Terminer la boucle si aucune cellule supplémentaire à traiter. */
        }
    }
}

/**
 * @brief Calcule les coûts des terrains sur la carte en utilisant un parcours en largeur (BFS).
 * @param map La carte.
 */
void calculateCosts(t_map map) {
    t_position baseStation = getBaseStationPosition(map); /**< Récupération de la position de la station de base. */
    t_queue file = createQueue(map.x_max * map.y_max);   /**< Création d'une file d'attente pour le BFS. */
    enqueue(&file, baseStation);                         /**< Enqueue de la station de base dans la file. */

    // Parcours en largeur (BFS) pour calculer les coûts
    while (file.first != file.last) {
        t_position pos = dequeue(&file); /**< Déqueue de la position actuelle. */
        int cout_self = _soil_cost[map.soils[pos.y][pos.x]]; /**< Coût du sol actuel. */

        // Déplacement vers les quatre directions
        t_position gauche = LEFT(pos);
        t_position droite = RIGHT(pos);
        t_position haut = UP(pos);
        t_position bas = DOWN(pos);

        int cout_min = COST_UNDEF;

        // Trouver le coût minimal parmi les voisins
        if (isValidLocalisation(gauche, map.x_max, map.y_max)) {
            cout_min = (map.costs[gauche.y][gauche.x] < cout_min) ? map.costs[gauche.y][gauche.x] : cout_min;
        }
        if (isValidLocalisation(droite, map.x_max, map.y_max)) {
            cout_min = (map.costs[droite.y][droite.x] < cout_min) ? map.costs[droite.y][droite.x] : cout_min;
        }
        if (isValidLocalisation(haut, map.x_max, map.y_max)) {
            cout_min = (map.costs[haut.y][haut.x] < cout_min) ? map.costs[haut.y][haut.x] : cout_min;
        }
        if (isValidLocalisation(bas, map.x_max, map.y_max)) {
            cout_min = (map.costs[bas.y][bas.x] < cout_min) ? map.costs[bas.y][bas.x] : cout_min;
        }

        // Calcul du coût total pour la cellule actuelle
        map.costs[pos.y][pos.x] = (map.soils[pos.y][pos.x] == BASE_STATION) ? 0 : cout_min + cout_self;

        // Enqueue des voisins si leur coût est indéfini
        if (isValidLocalisation(gauche, map.x_max, map.y_max) && map.costs[gauche.y][gauche.x] == COST_UNDEF) {
            map.costs[gauche.y][gauche.x] = COST_UNDEF - 1; /**< Marque comme en cours de traitement. */
            enqueue(&file, gauche);                     /**< Enqueue de la position à gauche. */
        }
        if (isValidLocalisation(droite, map.x_max, map.y_max) && map.costs[droite.y][droite.x] == COST_UNDEF) {
            map.costs[droite.y][droite.x] = COST_UNDEF - 1; /**< Marque comme en cours de traitement. */
            enqueue(&file, droite);                       /**< Enqueue de la position à droite. */
        }
        if (isValidLocalisation(haut, map.x_max, map.y_max) && map.costs[haut.y][haut.x] == COST_UNDEF) {
            map.costs[haut.y][haut.x] = COST_UNDEF - 1; /**< Marque comme en cours de traitement. */
            enqueue(&file, haut);                       /**< Enqueue de la position en haut. */
        }
        if (isValidLocalisation(bas, map.x_max, map.y_max) && map.costs[bas.y][bas.x] == COST_UNDEF) {
            map.costs[bas.y][bas.x] = COST_UNDEF - 1; /**< Marque comme en cours de traitement. */
            enqueue(&file, bas);                       /**< Enqueue de la position en bas. */
        }
    }
}

/**
 * @brief Crée une carte à partir d'un fichier.
 * @param filename Nom du fichier contenant la carte.
 * @return La carte créée.
 */
t_map createMapFromFile(char *filename) {
    t_map carte;
    int xdim, ydim;
    FILE *fichier = fopen(filename, "rt"); /**< Ouverture du fichier en mode texte. */
    if (fichier == NULL) {
        fprintf(stderr, "Error: cannot open file %s\n", filename);
        exit(1); /**< Terminer le programme si le fichier ne peut pas être ouvert. */
    }

    // Lecture des dimensions de la carte depuis le fichier
    fscanf(fichier, "%d", &ydim);
    fscanf(fichier, "%d", &xdim);
    carte.x_max = xdim;
    carte.y_max = ydim;

    // Allocation dynamique de la matrice des sols
    carte.soils = (t_soil **)malloc(ydim * sizeof(t_soil *));
    for (int i = 0; i < ydim; i++) {
        carte.soils[i] = (t_soil *)malloc(xdim * sizeof(t_soil));
    }

    // Allocation dynamique de la matrice des coûts
    carte.costs = (int **)malloc(ydim * sizeof(int *));
    for (int i = 0; i < ydim; i++) {
        carte.costs[i] = (int *)malloc(xdim * sizeof(int));
    }

    // Lecture des types de sols et initialisation des coûts
    for (int i = 0; i < ydim; i++) {
        for (int j = 0; j < xdim; j++) {
            int valeur;
            fscanf(fichier, "%d", &valeur);
            carte.soils[i][j] = valeur; /**< Assignation du type de sol. */
            carte.costs[i][j] = (valeur == BASE_STATION) ? 0 : COST_UNDEF; /**< Initialisation du coût. */
        }
    }

    fclose(fichier); /**< Fermeture du fichier. */

    calculateCosts(carte);        /**< Calcul des coûts des terrains. */
    removeFalseCrevasses(carte);  /**< Suppression des fausses crevasses. */
    return carte;                  /**< Retourne la carte créée. */
}

/**
 * @brief Crée une carte d'entraînement.
 * @return La carte d'entraînement créée.
 */
t_map createTrainingMap() {
    return createMapFromFile("..\\maps\\training.map"); /**< Appel à createMapFromFile avec le fichier training.map. */
}

/**
 * @brief Affiche la carte avec la position actuelle du rover.
 * @param map La carte à afficher.
 * @param marc_loc La localisation actuelle du rover.
 */
void displayMap(t_map map, t_localisation marc_loc) {
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            char c[4];
            // Vérification si la cellule courante correspond à la position du rover
            if (i == marc_loc.pos.y && j == marc_loc.pos.x) {
                strcpy(c, " R "); /**< Représentation du rover. */
            } else {
                // Représentation du type de sol
                switch (map.soils[i][j]) {
                    case BASE_STATION:
                        strcpy(c, " B "); /**< Station de base. */
                        break;
                    case PLAIN:
                        strcpy(c, " - "); /**< Plaine. */
                        break;
                    case ERG:
                        strcpy(c, " ~ "); /**< Dunes de sable. */
                        break;
                    case REG:
                        strcpy(c, " ^ "); /**< Terrain rocailleux. */
                        break;
                    case CREVASSE:
                        strcpy(c, " X "); /**< Crevasse. */
                        break;
                    case PENTE:
                        strcpy(c, " / "); /**< Terrain en pente. */
                        break;
                    default:
                        strcpy(c, " ? "); /**< Type de sol inconnu. */
                        break;
                }
            }
            printf("%s", c); /**< Affichage de la cellule. */
        }
        printf("\n"); /**< Nouvelle ligne après chaque ligne de la carte. */
    }
}
