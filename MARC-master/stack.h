#ifndef STACK_H
#define STACK_H

/**
 * @struct s_stack
 * @brief Structure représentant une pile.
 */
typedef struct s_stack {
    int *values; /**< Tableau contenant les éléments de la pile. */
    int size;    /**< Taille maximale de la pile. */
    int nbElts;  /**< Nombre actuel d'éléments dans la pile. */
} t_stack;

/**
 * @brief Crée une pile avec une taille spécifiée.
 * @param size Taille maximale de la pile.
 * @return La pile créée.
 */
t_stack createStack(int size);

/**
 * @brief Ajoute une valeur au sommet de la pile.
 * @param p_stack Pointeur vers la pile.
 * @param value Valeur à ajouter.
 */
void push(t_stack *p_stack, int value);

/**
 * @brief Retire et retourne la valeur au sommet de la pile.
 * @param p_stack Pointeur vers la pile.
 * @return La valeur retirée.
 */
int pop(t_stack *p_stack);

/**
 * @brief Retourne la valeur au sommet de la pile sans la retirer.
 * @param stack La pile.
 * @return La valeur au sommet de la pile.
 */
int top(t_stack stack);

#endif // STACK_H
