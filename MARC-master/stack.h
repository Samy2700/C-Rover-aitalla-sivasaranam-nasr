#ifndef STACK_H
#define STACK_H

/**
 * @struct s_stack
 * @brief Structure représentant une pile.
 */
typedef struct s_stack {
    int *values; // Tableau contenant les éléments de la pile.
    int size;    // Taille maximale de la pile.
    int nbElts;  // Nombre actuel d'éléments dans la pile.
} t_stack;

// Crée une pile avec une taille spécifiée.
t_stack createStack(int size);

// Ajoute une valeur au sommet de la pile.
void push(t_stack *p_stack, int value);

// Retire et retourne la valeur au sommet de la pile.
int pop(t_stack *p_stack);

// Retourne la valeur au sommet de la pile sans la retirer.
int top(t_stack stack);

#endif // STACK_H
