#include <assert.h>
#include <stdlib.h>
#include "queue.h"

/**
 * @brief Crée une file d'attente avec une taille spécifiée.
 * @param size Taille maximale de la file.
 * @return La file d'attente créée.
 */
t_queue createQueue(int size) {
    assert(size > 0); /**< Vérifie que la taille est positive. */
    t_queue file;
    file.size = size;      /**< Assignation de la taille maximale de la file. */
    file.first = 0;        /**< Initialisation de l'indice du premier élément. */
    file.last = 0;         /**< Initialisation de l'indice du dernier élément. */
    file.values = (t_position *)malloc(size * sizeof(t_position)); /**< Allocation dynamique du tableau des positions. */
    return file; /**< Retourne la file d'attente initialisée. */
}

/**
 * @brief Ajoute une position à la fin de la file.
 * @param p_queue Pointeur vers la file d'attente.
 * @param pos Position à ajouter.
 */
void enqueue(t_queue *p_queue, t_position pos) {
    assert((p_queue->last - p_queue->first) < p_queue->size); /**< Vérifie qu'il reste de la place dans la file. */
    p_queue->values[(p_queue->last) % p_queue->size] = pos; /**< Ajoute la position à la fin. */
    p_queue->last++; /**< Incrémente l'indice du dernier élément. */
}

/**
 * @brief Retire et retourne la position au début de la file.
 * @param p_queue Pointeur vers la file d'attente.
 * @return La position retirée.
 */
t_position dequeue(t_queue *p_queue) {
    assert(p_queue->last != p_queue->first); /**< Vérifie que la file n'est pas vide. */
    p_queue->first++; /**< Incrémente l'indice du premier élément. */
    return p_queue->values[(p_queue->first - 1) % p_queue->size]; /**< Retourne la position retirée. */
}
