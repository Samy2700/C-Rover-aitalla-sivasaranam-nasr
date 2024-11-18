#include "moves.h"
#include <string.h>

t_orientation rotate(t_orientation ori, t_move move) {
    switch (move) {
        case T_LEFT:
            return (ori + 3) % 4; // Rotation à gauche (-90 degrés)
        case T_RIGHT:
            return (ori + 1) % 4; // Rotation à droite (+90 degrés)
        case U_TURN:
            return (ori + 2) % 4; // Demi-tour (180 degrés)
        default:
            return ori; // Pas de rotation pour les mouvements de déplacement
    }
}


t_localisation translate(t_localisation loc, t_move move) {
    t_position res = loc.pos;
    switch (move) {
        case F_10:
            if (loc.ori == NORTH) res.y -= 1;
            else if (loc.ori == EAST) res.x += 1;
            else if (loc.ori == SOUTH) res.y += 1;
            else if (loc.ori == WEST) res.x -= 1;
            break;
        case F_20:
            if (loc.ori == NORTH) res.y -= 2;
            else if (loc.ori == EAST) res.x += 2;
            else if (loc.ori == SOUTH) res.y += 2;
            else if (loc.ori == WEST) res.x -= 2;
            break;
        case F_30:
            if (loc.ori == NORTH) res.y -= 3;
            else if (loc.ori == EAST) res.x += 3;
            else if (loc.ori == SOUTH) res.y += 3;
            else if (loc.ori == WEST) res.x -= 3;
            break;
        case B_10:
            if (loc.ori == NORTH) res.y += 1;
            else if (loc.ori == EAST) res.x -= 1;
            else if (loc.ori == SOUTH) res.y -= 1;
            else if (loc.ori == WEST) res.x += 1;
            break;
        case B_20:
            if (loc.ori == NORTH) res.y += 2;
            else if (loc.ori == EAST) res.x -= 2;
            else if (loc.ori == SOUTH) res.y -= 2;
            else if (loc.ori == WEST) res.x += 2;
            break;
        default:
            break;
    }
    loc.pos = res;
    return loc;
}


char *getMoveAsString(t_move move) {
    if (move >= 0 && move < 9) {
        return _moves[move];
    }
    return "Unknown";
}

t_localisation performMove(t_localisation loc, t_move move_cmd) {
    // Mettre à jour l'orientation si le mouvement est une rotation
    if (move_cmd == T_LEFT || move_cmd == T_RIGHT || move_cmd == U_TURN) {
        loc.ori = rotate(loc.ori, move_cmd);
    }

    // Mettre à jour la position si le mouvement est une translation
    if (move_cmd == F_10 || move_cmd == F_20 || move_cmd == F_30 || move_cmd == B_10 || move_cmd == B_20) {
        loc = translate(loc, move_cmd);
    }

    return loc;
}


void updateLocalisation(t_localisation *p_loc, t_move m) {
    *p_loc = performMove(*p_loc, m);
}
