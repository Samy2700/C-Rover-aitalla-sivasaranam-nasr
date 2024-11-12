#ifndef MOVES_H
#define MOVES_H

#include "loc.h"

static char _moves[7][8] = {"F 10m", "F 20m", "F 30m", "B 10m", "T left", "T right", "U-turn"};

typedef enum e_move {
    F_10,
    F_20,
    F_30,
    B_10,
    T_LEFT,
    T_RIGHT,
    U_TURN
} t_move;

char *getMoveAsString(t_move move);
t_localisation performMove(t_localisation loc, t_move move_cmd);
void updateLocalisation(t_localisation *p_loc, t_move m);

#endif // MOVES_H
