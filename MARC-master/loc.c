#include "loc.h"

/**
 * @brief Initialise une localisation avec les coordonnées x, y et l'orientation ori.
 * @param x Coordonnée en abscisse.
 * @param y Coordonnée en ordonnée.
 * @param ori Orientation initiale.
 * @return La localisation initialisée.
 */
t_localisation loc_init(int x, int y, t_orientation ori) {
    t_localisation localisation;
    localisation.pos.x = x;     // Définition de la coordonnée x.
    localisation.pos.y = y;     // Définition de la coordonnée y.
    localisation.ori = ori;     // Définition de l'orientation.
    return localisation;        // Retour de la localisation initialisée.
}

/**
 * @brief Vérifie si la position loc est à l'intérieur des limites de la carte définies par x_max et y_max.
 * @param loc Position à vérifier.
 * @param x_max Limite maximale en abscisse.
 * @param y_max Limite maximale en ordonnée.
 * @return 1 si la position est valide, 0 sinon.
 */
int isValidLocalisation(t_position loc, int x_max, int y_max) {
    // Vérifie que les coordonnées sont dans les limites.
    return (loc.x >= 0 && loc.x < x_max && loc.y >= 0 && loc.y < y_max);
}

/**
 * @brief Déplace la position vers la gauche (diminue la coordonnée x de 1).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers la gauche.
 */
t_position LEFT(t_position pos) {
    t_position nouvelle_position;
    nouvelle_position.x = pos.x - 1; // Déplacement vers la gauche.
    nouvelle_position.y = pos.y;     // Coordonnée y reste inchangée.
    return nouvelle_position;        // Retour de la nouvelle position.
}

/**
 * @brief Déplace la position vers la droite (augmente la coordonnée x de 1).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers la droite.
 */
t_position RIGHT(t_position pos) {
    t_position nouvelle_position;
    nouvelle_position.x = pos.x + 1; // Déplacement vers la droite.
    nouvelle_position.y = pos.y;     // Coordonnée y reste inchangée.
    return nouvelle_position;        // Retour de la nouvelle position.
}

/**
 * @brief Déplace la position vers le haut (diminue la coordonnée y de 1).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers le haut.
 */
t_position UP(t_position pos) {
    t_position nouvelle_position;
    nouvelle_position.x = pos.x;     // Coordonnée x reste inchangée.
    nouvelle_position.y = pos.y - 1; // Déplacement vers le haut.
    return nouvelle_position;        // Retour de la nouvelle position.
}

/**
 * @brief Déplace la position vers le bas (augmente la coordonnée y de 1).
 * @param pos Position actuelle.
 * @return Nouvelle position après déplacement vers le bas.
 */
t_position DOWN(t_position pos) {
    t_position nouvelle_position;
    nouvelle_position.x = pos.x;     // Coordonnée x reste inchangée.
    nouvelle_position.y = pos.y + 1; // Déplacement vers le bas.
    return nouvelle_position;        // Retour de la nouvelle position.
}
