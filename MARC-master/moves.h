#ifndef MOVES_H
#define MOVES_H

#include "loc.h"

// Description des mouvements
static char _moves[9][12] = {
        "F 10m", "F 20m", "F 30m",
        "B 10m", "B 20m",
        "T left", "T right", "U-turn",
        "L 90m" // Nouveau mouvement : Lever 90 degrés
};

// Types de mouvements
typedef enum e_move {
    F_10,
    F_20,
    F_30,
    B_10,
    B_20,
    T_LEFT,
    T_RIGHT,
    U_TURN,
    L_90
} t_move;

// Récupère la description d'un mouvement
char *getMoveAsString(t_move move);

// Effectue un mouvement et retourne la nouvelle localisation
t_localisation performMove(t_localisation loc, t_move move_cmd);

// Met à jour la localisation du rover avec un mouvement donné
void updateLocalisation(t_localisation *p_loc, t_move m);

#endif // MOVES_H
