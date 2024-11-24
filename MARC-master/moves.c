#include "moves.h"
#include <string.h>
#include <stdio.h>

/**
 * @brief Tourne le rover en fonction du mouvement.
 * @param ori Orientation actuelle.
 * @param move Mouvement de rotation à effectuer.
 * @return La nouvelle orientation après rotation.
 */
t_orientation rotate(t_orientation ori, t_move move) {
    switch (move) {
        case T_LEFT:
            return (ori + 3) % 4; /**< Rotation à gauche (-90 degrés). */
        case T_RIGHT:
            return (ori + 1) % 4; /**< Rotation à droite (+90 degrés). */
        case U_TURN:
            return (ori + 2) % 4; /**< Demi-tour (180 degrés). */
        default:
            return ori; /**< Pas de rotation pour les mouvements de déplacement. */
    }
}

/**
 * @brief Traduit une localisation en une nouvelle localisation après un mouvement de translation.
 * @param loc Localisation actuelle.
 * @param move Mouvement de translation à effectuer.
 * @return La nouvelle localisation après translation.
 */
t_localisation translate(t_localisation loc, t_move move) {
    t_position res = loc.pos;
    switch (move) {
        case F_10:
            if (loc.ori == NORTH) res.y -= 1; /**< Avancer vers le nord. */
            else if (loc.ori == EAST) res.x += 1; /**< Avancer vers l'est. */
            else if (loc.ori == SOUTH) res.y += 1; /**< Avancer vers le sud. */
            else if (loc.ori == WEST) res.x -= 1; /**< Avancer vers l'ouest. */
            break;
        case F_20:
            if (loc.ori == NORTH) res.y -= 2; /**< Avancer de 2 vers le nord. */
            else if (loc.ori == EAST) res.x += 2; /**< Avancer de 2 vers l'est. */
            else if (loc.ori == SOUTH) res.y += 2; /**< Avancer de 2 vers le sud. */
            else if (loc.ori == WEST) res.x -= 2; /**< Avancer de 2 vers l'ouest. */
            break;
        case F_30:
            if (loc.ori == NORTH) res.y -= 3; /**< Avancer de 3 vers le nord. */
            else if (loc.ori == EAST) res.x += 3; /**< Avancer de 3 vers l'est. */
            else if (loc.ori == SOUTH) res.y += 3; /**< Avancer de 3 vers le sud. */
            else if (loc.ori == WEST) res.x -= 3; /**< Avancer de 3 vers l'ouest. */
            break;
        case B_10:
            if (loc.ori == NORTH) res.y += 1; /**< Reculer vers le sud. */
            else if (loc.ori == EAST) res.x -= 1; /**< Reculer vers l'ouest. */
            else if (loc.ori == SOUTH) res.y -= 1; /**< Reculer vers le nord. */
            else if (loc.ori == WEST) res.x += 1; /**< Reculer vers l'est. */
            break;
        case B_20:
            if (loc.ori == NORTH) res.y += 2; /**< Reculer de 2 vers le sud. */
            else if (loc.ori == EAST) res.x -= 2; /**< Reculer de 2 vers l'ouest. */
            else if (loc.ori == SOUTH) res.y -= 2; /**< Reculer de 2 vers le nord. */
            else if (loc.ori == WEST) res.x += 2; /**< Reculer de 2 vers l'est. */
            break;
        default:
            break;
    }
    loc.pos = res; /**< Mise à jour de la position. */
    return loc;    /**< Retourne la nouvelle localisation après le mouvement. */
}

/**
 * @brief Obtient la description d'un mouvement sous forme de chaîne de caractères.
 * @param move Le mouvement dont on veut la description.
 * @return La chaîne de caractères décrivant le mouvement.
 */
char *getMoveAsString(t_move move) {
    if (move >= 0 && move < 9) {
        return _moves[move]; /**< Retourne la chaîne correspondant au mouvement. */
    }
    return "Unknown"; /**< Retourne "Unknown" si le mouvement n'est pas valide. */
}

/**
 * @brief Effectue un mouvement et retourne la nouvelle localisation.
 * @param loc La localisation actuelle du rover.
 * @param move_cmd Le mouvement à effectuer.
 * @return La nouvelle localisation après le mouvement.
 */
t_localisation performMove(t_localisation loc, t_move move_cmd) {
    // Mettre à jour l'orientation si le mouvement est une rotation
    if (move_cmd == T_LEFT || move_cmd == T_RIGHT || move_cmd == U_TURN) {
        loc.ori = rotate(loc.ori, move_cmd);
    }

    // Mettre à jour la position si le mouvement est une translation
    if (move_cmd == F_10 || move_cmd == F_20 || move_cmd == F_30 ||
        move_cmd == B_10 || move_cmd == B_20) {
        loc = translate(loc, move_cmd);
    }

    // Si le mouvement est un scan (L_90), afficher un message
    if (move_cmd == L_90) {
        printf("MARC is scanning the environment.\n");
    }

    return loc; /**< Retourne la nouvelle localisation après le mouvement. */
}

/**
 * @brief Met à jour la localisation du rover avec un mouvement donné.
 * @param p_loc Pointeur vers la localisation actuelle du rover.
 * @param m Le mouvement à effectuer.
 */
void updateLocalisation(t_localisation *p_loc, t_move m) {
    *p_loc = performMove(*p_loc, m); /**< Mise à jour de la localisation en appliquant le mouvement. */
}
