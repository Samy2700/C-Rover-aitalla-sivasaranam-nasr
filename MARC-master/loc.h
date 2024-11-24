#ifndef LOC_H
#define LOC_H

/**
 * @enum e_orientation
 * @brief Enumération des orientations possibles du rover.
 */
typedef enum e_orientation {
    NORTH, // Nord
    EAST,  // Est
    SOUTH, // Sud
    WEST   // Ouest
} t_orientation;

/**
 * @struct e_position
 * @brief Structure représentant une position sur la carte.
 */
typedef struct e_position {
    int x; // Coordonnée en abscisse (x).
    int y; // Coordonnée en ordonnée (y).
} t_position;

/**
 * @struct s_localisation
 * @brief Structure représentant la localisation du rover, incluant sa position et son orientation.
 */
typedef struct s_localisation {
    t_position pos;    // Position actuelle du rover.
    t_orientation ori; // Orientation actuelle du rover.
} t_localisation;

// Initialise la localisation du rover avec les coordonnées (x, y) et l'orientation ori.
t_localisation loc_init(int x, int y, t_orientation ori);

// Vérifie si la localisation est valide à l'intérieur des limites de la carte (x_max, y_max).
int isValidLocalisation(t_position loc, int x_max, int y_max);

// Déplace la position vers la gauche (Ouest).
t_position LEFT(t_position pos);

// Déplace la position vers la droite (Est).
t_position RIGHT(t_position pos);

// Déplace la position vers le haut (Nord).
t_position UP(t_position pos);

// Déplace la position vers le bas (Sud).
t_position DOWN(t_position pos);

#endif // LOC_H
