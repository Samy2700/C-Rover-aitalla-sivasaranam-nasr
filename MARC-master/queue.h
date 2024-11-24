#ifndef QUEUE_H
#define QUEUE_H
#include "loc.h"

/**
 * @struct s_queue
 * @brief Structure représentant une file d'attente.
 */
typedef struct s_queue {
    t_position *values; // Tableau contenant les positions dans la file.
    int size;           // Taille maximale de la file.
    int last;           // Indice du dernier élément ajouté.
    int first;          // Indice du premier élément à retirer.
} t_queue;

// Crée une file d'attente avec une taille spécifiée.
t_queue createQueue(int size);

// Ajoute une position à la fin de la file.
void enqueue(t_queue *p_queue, t_position pos);

// Retire et retourne la position au début de la file.
t_position dequeue(t_queue *p_queue);

#endif // QUEUE_H
