#ifndef QUEUE_H
#define QUEUE_H
#include "loc.h"

/**
 * @struct s_queue
 * @brief Structure représentant une file d'attente.
 */
typedef struct s_queue {
    t_position *values; /**< Tableau contenant les positions dans la file. */
    int size;           /**< Taille maximale de la file. */
    int last;           /**< Indice du dernier élément ajouté. */
    int first;          /**< Indice du premier élément à retirer. */
} t_queue;

/**
 * @brief Crée une file d'attente avec une taille spécifiée.
 * @param size Taille maximale de la file.
 * @return La file d'attente créée.
 */
t_queue createQueue(int size);

/**
 * @brief Ajoute une position à la fin de la file.
 * @param p_queue Pointeur vers la file d'attente.
 * @param pos Position à ajouter.
 */
void enqueue(t_queue *p_queue, t_position pos);

/**
 * @brief Retire et retourne la position au début de la file.
 * @param p_queue Pointeur vers la file d'attente.
 * @return La position retirée.
 */
t_position dequeue(t_queue *p_queue);

#endif // QUEUE_H
