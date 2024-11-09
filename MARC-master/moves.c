#include "moves.h"

t_orientation rotate(t_orientation ori, t_move move) {
    int rst;
    switch (move) {
        case T_LEFT:
            rst = 3;
            break;
        case T_RIGHT:
            rst = 1;
            break;
        case U_TURN:
            rst = 2;
            break;
        default:
            rst = 0;
            break;
    }
    return (ori + rst) % 4;
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
        default:
            break;
    }
    return loc_init(res.x, res.y, loc.ori);
}

char *getMoveAsString(t_move move) {
    return _moves[move];
}

t_localisation move(t_localisation loc, t_move move_cmd) {
    t_localisation new_loc;
    new_loc.ori = rotate(loc.ori, move_cmd);
    new_loc = translate(loc, move_cmd);
    return new_loc;
}

void updateLocalisation(t_localisation *p_loc, t_move m) {
    *p_loc = move(*p_loc, m);
}
