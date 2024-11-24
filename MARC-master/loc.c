#include "loc.h"

// Initialise une localisation avec une position donnée (x, y) et une orientation initiale.
t_localisation loc_init(int x, int y, t_orientation ori) {
    t_localisation loc;
    loc.pos.x = x;      // Initialise la coordonnée x
    loc.pos.y = y;      // Initialise la coordonnée y
    loc.ori = ori;      // Définit l'orientation initiale
    return loc;         // Retourne la localisation initialisée
}

// Vérifie si une position donnée (loc) est valide dans un espace 2D délimité par (x_max, y_max).
int isValidLocalisation(t_position loc, int x_max, int y_max) {
    return (loc.x >= 0 && loc.x < x_max && loc.y >= 0 && loc.y < y_max);
    // Retourne 1 si la position est valide, 0 sinon
}

// Déplace la position donnée (pos) d'une unité vers la gauche.
t_position LEFT(t_position pos) {
    t_position new_pos;
    new_pos.x = pos.x - 1; // Réduit la coordonnée x de 1
    new_pos.y = pos.y;     // Garde la coordonnée y inchangée
    return new_pos;        // Retourne la nouvelle position
}

// Déplace la position donnée (pos) d'une unité vers la droite.
t_position RIGHT(t_position pos) {
    t_position new_pos;
    new_pos.x = pos.x + 1; // Augmente la coordonnée x de 1
    new_pos.y = pos.y;     // Garde la coordonnée y inchangée
    return new_pos;        // Retourne la nouvelle position
}

// Déplace la position donnée (pos) d'une unité vers le haut.
t_position UP(t_position pos) {
    t_position new_pos;
    new_pos.x = pos.x;     // Garde la coordonnée x inchangée
    new_pos.y = pos.y - 1; // Réduit la coordonnée y de 1
    return new_pos;        // Retourne la nouvelle position
}

// Déplace la position donnée (pos) d'une unité vers le bas.
t_position DOWN(t_position pos) {
    t_position new_pos;
    new_pos.x = pos.x;     // Garde la coordonnée x inchangée
    new_pos.y = pos.y + 1; // Augmente la coordonnée y de 1
    return new_pos;        // Retourne la nouvelle position
}
