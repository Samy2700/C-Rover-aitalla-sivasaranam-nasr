#ifndef MAP_H
#define MAP_H

#define COST_UNDEF 65535

#include "loc.h" // Assurez-vous que loc.h est inclus pour utiliser t_position

typedef enum e_soil {
    BASE_STATION,
    PLAIN,
    ERG,
    REG,
    CREVASSE
} t_soil;

static const int _soil_cost[5] = {0, 1, 2, 4, 10000};

typedef struct s_map {
    t_soil  **soils;
    int     **costs;
    int     x_max;
    int     y_max;
} t_map;

// Déclarations des fonctions accessibles depuis d'autres fichiers
t_map createMapFromFile(char *filename);
t_map createTrainingMap();
void displayMap(t_map map, t_localisation marc_loc);
t_position getBaseStationPosition(t_map map); // Ajout de la déclaration

#endif // MAP_H
