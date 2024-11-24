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
    int x; /**< Coordonnée en abscisse (x). */
    int y; /**< Coordonnée en ordonnée (y). */
} t_position;

/**
 * @struct s_localisation
 * @brief Structure représentant la localisation du rover, incluant sa position et son orientation.
 */
typedef struct s_localisation {
    t_position pos;    /**< Position actuelle du rover. */
    t_orientation ori; /**< Orientation actuelle du rover. */
} t_localisation;

/**
 * @brief Initialise la localisation du rover avec les coordonnées (x, y) et l'orientation ori.
 * @param x Coordonnée en abscisse.
 * @param y Coordonnée en ordonnée.
 * @param ori Orientation initiale.
 * @return La localisation initialisée.
 */
t_localisation loc_init(int x, int y, t_orientation ori);

/**
 * @brief Vérifie si la localisation est valide à l'intérieur des limites de la carte (x_max, y_max).
 * @param loc Position à vérifier.
 * @param x_max Limite maximale en abscisse.
 * @param y_max Limite maximale en ordonnée.
 * @return 1 si la position est valide, 0 sinon.
 */
int isValidLocalisation(t_position loc, int x_max, int y_max);

/**
 * @brief Déplace la position vers la gauche (Ouest).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers la gauche.
 */
t_position LEFT(t_position pos);

/**
 * @brief Déplace la position vers la droite (Est).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers la droite.
 */
t_position RIGHT(t_position pos);

/**
 * @brief Déplace la position vers le haut (Nord).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers le haut.
 */
t_position UP(t_position pos);

/**
 * @brief Déplace la position vers le bas (Sud).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers le bas.
 */
t_position DOWN(t_position pos);

#endif // LOC_H
