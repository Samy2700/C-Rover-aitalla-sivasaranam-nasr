#ifndef MAP_H
#define MAP_H

#define COST_UNDEF 65535 /**< Valeur représentant un coût indéfini ou inaccessible. */

#include "loc.h" /**< Inclusion de loc.h pour utiliser t_position. */

/**
 * @enum e_soil
 * @brief Enumération des types de sols disponibles sur la carte.
 */
typedef enum e_soil {
    BASE_STATION, /**< Station de base. */
    PLAIN,        /**< Plaine. */
    ERG,          /**< Dunes de sable (erg). */
    REG,          /**< Terrain rocailleux (reg). */
    CREVASSE,     /**< Crevasse. */
    PENTE         /**< Terrain en pente. */
} t_soil;

/**
 * @brief Tableau des coûts associés à chaque type de sol.
 */
static const int _soil_cost[6] = {0, 1, 2, 4, 10000, 3};

/**
 * @struct s_map
 * @brief Structure représentant la carte, avec les sols, les coûts, et les dimensions.
 */
typedef struct s_map {
    t_soil  **soils;  /**< Matrice des types de sols. */
    int     **costs;   /**< Matrice des coûts des terrains. */
    int     x_max;     /**< Nombre maximal de colonnes (abscisse). */
    int     y_max;     /**< Nombre maximal de lignes (ordonnée). */
} t_map;

/**
 * @brief Crée une carte à partir d'un fichier spécifié par filename.
 * @param filename Nom du fichier contenant la carte.
 * @return La carte créée.
 */
t_map createMapFromFile(char *filename);

/**
 * @brief Crée une carte d'entraînement (training map).
 * @return La carte d'entraînement créée.
 */
t_map createTrainingMap();

/**
 * @brief Affiche la carte avec la position actuelle du rover.
 * @param map La carte à afficher.
 * @param marc_loc La localisation actuelle du rover.
 */
void displayMap(t_map map, t_localisation marc_loc);

/**
 * @brief Récupère la position de la station de base sur la carte.
 * @param map La carte.
 * @return La position de la station de base.
 */
t_position getBaseStationPosition(t_map map); // Ajout de la déclaration

#endif // MAP_H
