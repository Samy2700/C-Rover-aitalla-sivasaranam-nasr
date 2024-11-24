#ifndef MAP_H
#define MAP_H

#define COST_UNDEF 65535 // Valeur représentant un coût indéfini ou inaccessible.

#include "loc.h"

/**
 * @enum e_soil
 * @brief Enumération des types de sols disponibles sur la carte.
 */
typedef enum e_soil {
    BASE_STATION, // Station de base.
    PLAIN,        // Plaine.
    ERG,          // Dunes de sable (erg).
    REG,          // Terrain rocailleux (reg).
    CREVASSE,     // Crevasse.
    PENTE         // Terrain en pente.
} t_soil;

// Tableau des coûts associés à chaque type de sol.
static const int _soil_cost[6] = {0, 1, 2, 4, 10000, 3};

/**
 * @struct s_map
 * @brief Structure représentant la carte, avec les sols, les coûts, et les dimensions.
 */
typedef struct s_map {
    t_soil  **soils;  // Matrice des types de sols.
    int     **costs;   // Matrice des coûts des terrains.
    int     x_max;     // Nombre maximal de colonnes (abscisse).
    int     y_max;     // Nombre maximal de lignes (ordonnée).
} t_map;

// Crée une carte à partir d'un fichier spécifié par filename.
t_map createMapFromFile(char *filename);

// Crée une carte d'entraînement (training map).
t_map createTrainingMap();

// Affiche la carte avec la position actuelle du rover.
void displayMap(t_map map, t_localisation marc_loc);

// Récupère la position de la station de base sur la carte.
t_position getBaseStationPosition(t_map map);

#endif // MAP_H
