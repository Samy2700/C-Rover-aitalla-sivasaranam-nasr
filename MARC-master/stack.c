#include <stdlib.h>
#include <assert.h>
#include "stack.h"

/**
 * @brief Crée une pile avec une taille spécifiée.
 * @param size Taille maximale de la pile.
 * @return La pile créée.
 */
t_stack createStack(int size) {
    assert(size > 0); // Vérifie que la taille est positive.
    t_stack pile;
    pile.size = size;       // Assignation de la taille maximale de la pile.
    pile.nbElts = 0;        // Initialisation du nombre d'éléments dans la pile.
    pile.values = (int *)malloc(size * sizeof(int)); // Allocation dynamique du tableau des éléments.
    return pile; // Retourne la pile initialisée.
}

/**
 * @brief Ajoute une valeur au sommet de la pile.
 * @param p_stack Pointeur vers la pile.
 * @param value Valeur à ajouter.
 */
void push(t_stack *p_stack, int value) {
    assert(p_stack->nbElts < p_stack->size); // Vérifie qu'il reste de la place dans la pile.
    p_stack->values[p_stack->nbElts] = value; // Ajoute la valeur au sommet.
    p_stack->nbElts++; // Incrémente le nombre d'éléments dans la pile.
}

/**
 * @brief Retire et retourne la valeur au sommet de la pile.
 * @param p_stack Pointeur vers la pile.
 * @return La valeur retirée.
 */
int pop(t_stack *p_stack) {
    assert(p_stack->nbElts > 0); // Vérifie que la pile n'est pas vide.
    p_stack->nbElts--; // Décrémente le nombre d'éléments dans la pile.
    return p_stack->values[p_stack->nbElts]; // Retourne la valeur retirée.
}

/**
 * @brief Retourne la valeur au sommet de la pile sans la retirer.
 * @param stack La pile.
 * @return La valeur au sommet de la pile.
 */
int top(t_stack stack) {
    assert(stack.nbElts > 0); // Vérifie que la pile n'est pas vide.
    return stack.values[stack.nbElts - 1]; // Retourne la valeur au sommet de la pile.
}
