#ifndef MOVES_H
#define MOVES_H

#include "loc.h"

// Description des mouvements disponibles.
static char _moves[9][12] = {
        "F 10m",  // Avancer de 10 mètres.
        "F 20m",  // Avancer de 20 mètres.
        "F 30m",  // Avancer de 30 mètres.
        "B 10m",  // Reculer de 10 mètres.
        "B 20m",  // Reculer de 20 mètres.
        "T left", // Tourner à gauche.
        "T right",// Tourner à droite.
        "U-turn", // Demi-tour.
        "L 90m"   // Lever (ou scanner) de 90 degrés.
};

/**
 * @enum e_move
 * @brief Énumération des types de mouvements.
 */
typedef enum e_move {
    F_10,   // Avancer de 10 mètres.
    F_20,   // Avancer de 20 mètres.
    F_30,   // Avancer de 30 mètres.
    B_10,   // Reculer de 10 mètres.
    B_20,   // Reculer de 20 mètres.
    T_LEFT, // Tourner à gauche.
    T_RIGHT,// Tourner à droite.
    U_TURN, // Demi-tour.
    L_90    // Lever (ou scanner) de 90 degrés.
} t_move;

// Obtient la description d'un mouvement sous forme de chaîne de caractères.
char *getMoveAsString(t_move move);

// Effectue un mouvement et retourne la nouvelle localisation du rover.
t_localisation performMove(t_localisation loc, t_move move_cmd);

// Met à jour la localisation du rover avec un mouvement donné.
void updateLocalisation(t_localisation *p_loc, t_move m);

#endif // MOVES_H
