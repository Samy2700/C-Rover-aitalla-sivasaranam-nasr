#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "tree.h"

#define MAX_DEPTH 3           /**< Profondeur maximale de l'arbre de mouvements. */
#define MAX_PATH_LENGTH 20    /**< Longueur maximale du chemin trouvé. */
#define MAX_ENERGY 500       /**< Énergie initiale du rover. */

int main() {
    srand(time(NULL)); /**< Initialisation de la graine pour les fonctions aléatoires. */
    int choix;

    // Affichage des options de cartes disponibles
    printf("Choisissez la carte à charger:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("3. custom1.map\n");
    printf("4. custom2.map\n");
    printf("Entrez votre choix (1, 2, 3 ou 4): ");
    scanf("%d", &choix);

    t_map carte;

    // Chargement de la carte choisie en fonction du système d'exploitation (Windows ou autre)
#if defined(_WIN32) || defined(_WIN64)
    if (choix == 1) {
        carte = createMapFromFile("..\\maps\\example1.map");
    } else if (choix == 2) {
        carte = createTrainingMap();
    } else if (choix == 3) {
        carte = createMapFromFile("..\\maps\\custom1.map");
    } else if (choix == 4) {
        carte = createMapFromFile("..\\maps\\custom2.map");
    } else {
        printf("Choix invalide. Chargement de example1.map par défaut.\n");
        carte = createMapFromFile("..\\maps\\example1.map");
    }
#else
    if (choix == 1) {
        carte = createMapFromFile("../maps/example1.map");
    } else if (choix == 2) {
        carte = createTrainingMap();
    } else if (choix == 3) {
        carte = createMapFromFile("../maps/custom1.map");
    } else if (choix == 4) {
        carte = createMapFromFile("../maps/custom2.map");
    } else {
        printf("Choix invalide. Chargement de example1.map par défaut.\n");
        carte = createMapFromFile("../maps/example1.map");
    }
#endif

    // Affichage des dimensions de la carte chargée
    printf("Carte creee avec des dimensions %d x %d\n", carte.y_max, carte.x_max);

    // Affichage des types de sols sur la carte
    printf("Affichage des types de sols:\n");
    for (int i = 0; i < carte.y_max; i++) {
        for (int j = 0; j < carte.x_max; j++) {
            printf("%d ", carte.soils[i][j]);
        }
        printf("\n");
    }

    // Affichage des coûts des terrains sur la carte
    printf("\nAffichage des couts:\n");
    for (int i = 0; i < carte.y_max; i++) {
        for (int j = 0; j < carte.x_max; j++) {
            printf("%-5d ", carte.costs[i][j]);
        }
        printf("\n");
    }

    // Initialisation de la position du rover
    t_localisation localisation_marc;
    int depart_trouve = 0; /**< Indicateur pour savoir si une position de départ valide est trouvée. */
    int cout_total = 0;    /**< Coût total accumulé. */
    int energie = MAX_ENERGY; /**< Énergie initiale du rover. */
    int effet_erg = 0;     /**< Effet du terrain ERG sur le prochain mouvement. */

    for (int i = 0; i < carte.y_max && !depart_trouve; i++) {
        for (int j = 0; j < carte.x_max && !depart_trouve; j++) {
            if (carte.soils[i][j] != BASE_STATION && carte.soils[i][j] != CREVASSE) {
                localisation_marc = loc_init(j, i, NORTH); /**< Initialisation de la localisation du rover. */
                cout_total = carte.costs[i][j];         /**< Coût initial basé sur la case de départ. */
                energie -= carte.costs[i][j];           /**< Décrémentation de l'énergie initiale. */
                depart_trouve = 1;                       /**< Position de départ trouvée. */
            }
        }
    }

    // Vérification si une position de départ valide a été trouvée
    if (!depart_trouve) {
        fprintf(stderr, "Erreur: Aucune position de depart valide trouvee.\n");
        // Libération de la mémoire allouée pour la carte
        for (int i = 0; i < carte.y_max; i++) {
            free(carte.soils[i]);
            free(carte.costs[i]);
        }
        free(carte.soils);
        free(carte.costs);
        exit(1); /**< Terminer le programme avec une erreur. */
    }

    // Affichage de la position initiale du rover
    printf("\nPosition initiale de MARC: (%d, %d), orientation: %d (NORD=0, EST=1, SUD=2, OUEST=3)\n",
           localisation_marc.pos.x, localisation_marc.pos.y, localisation_marc.ori);
    displayMap(carte, localisation_marc); /**< Affichage de la carte avec la position du rover. */

    int mission_completee = 0;    /**< Indicateur pour savoir si la mission est terminée. */
    int phase_actuelle = 0;        /**< Compteur des phases. */

    // Boucle principale de la mission
    while (!mission_completee) {
        printf("\n--- Phase %d ---\n", phase_actuelle + 1);

        // Affichage de l'énergie restante
        printf("Energie restante: %d\n", energie);

        // Demande à l'utilisateur de sélectionner le nombre de mouvements disponibles pour la phase suivante
        int nb_mouvements_disponibles = 0;
        printf("Entrez le nombre de mouvements disponibles pour la prochaine phase (jusqu'a 9): ");
        scanf("%d", &nb_mouvements_disponibles);

        // Validation du nombre de mouvements sélectionnés
        if (nb_mouvements_disponibles < 1 || nb_mouvements_disponibles > 9) {
            printf("Nombre de mouvements invalide. Defaut à 5 mouvements.\n");
            nb_mouvements_disponibles = 5;
        }

        // Affichage des mouvements disponibles pour la sélection
        printf("Selectionnez %d mouvements dans la liste ci-dessous en entrant leurs numeros separes par des espaces:\n", nb_mouvements_disponibles);
        for (int i = 0; i < 9; i++) {
            printf("%d. %s\n", i + 1, getMoveAsString(i));
        }

        int indices_mouvements_selectionnes[9]; /**< Indices des mouvements sélectionnés. */
        t_move mouvements_disponibles[9];      /**< Mouvements disponibles pour cette phase. */
        for (int i = 0; i < nb_mouvements_disponibles; i++) {
            scanf("%d", &indices_mouvements_selectionnes[i]); /**< Lecture des choix de l'utilisateur. */
            if (indices_mouvements_selectionnes[i] < 1 || indices_mouvements_selectionnes[i] > 9) {
                printf("Numero de mouvement invalide. Veuillez entrer un numero entre 1 et 9.\n");
                i--; /**< Redemander ce mouvement. */
            } else {
                mouvements_disponibles[i] = indices_mouvements_selectionnes[i] - 1; /**< Conversion en indice de l'énumération. */
            }
        }

        // Mesure du temps de construction de l'arbre des mouvements
        clock_t debut_construction = clock();
        t_treeNode *racine = createTreeNode(localisation_marc, cout_total, -1, NULL); /**< Création de la racine de l'arbre. */
        buildTree(racine, 0, MAX_DEPTH, mouvements_disponibles, nb_mouvements_disponibles, carte); /**< Construction de l'arbre. */
        clock_t fin_construction = clock();
        double temps_construction = ((double)(fin_construction - debut_construction)) / CLOCKS_PER_SEC; /**< Calcul du temps écoulé. */

        // Mesure du temps de recherche de la feuille avec le coût minimal
        clock_t debut_recherche = clock();
        t_treeNode *feuille_min = findMinCostLeaf(racine, NULL); /**< Recherche de la feuille minimale. */
        clock_t fin_recherche = clock();
        double temps_recherche = ((double)(fin_recherche - debut_recherche)) / CLOCKS_PER_SEC;

        // Mesure du temps de reconstruction du chemin optimal
        clock_t debut_chemin = clock();
        t_move chemin[MAX_PATH_LENGTH]; /**< Tableau pour stocker le chemin. */
        int longueur_chemin = 0;        /**< Longueur du chemin. */
        getPathFromLeaf(feuille_min, chemin, &longueur_chemin); /**< Reconstruction du chemin. */
        clock_t fin_chemin = clock();
        double temps_chemin = ((double)(fin_chemin - debut_chemin)) / CLOCKS_PER_SEC;

        // Affichage des métriques de performance
        printf("\n--- Metriques de Performance ---\n");
        printf("Temps de construction de l'arbre: %.6f secondes\n", temps_construction);
        printf("Temps de recherche de la feuille minimale: %.6f secondes\n", temps_recherche);
        printf("Temps de calcul du chemin: %.6f secondes\n", temps_chemin);
        printf("-------------------------------\n");

        // Affichage du chemin optimal trouvé
        printf("\nChemin optimal trouve avec un cout total de %d:\n", feuille_min->cost);
        for (int i = 0; i < longueur_chemin; i++) {
            printf("Étape %d: Mouvement %s\n", i + 1, getMoveAsString(chemin[i]));
        }

        // Application du premier mouvement du chemin optimal
        if (longueur_chemin == 0) {
            printf("Aucun mouvement disponible. Mission terminée.\n");
            mission_completee = 1;
            freeTree(racine);
            break;
        }

        t_move mouvement_selectionne = chemin[0];

        printf("\nExecution du mouvement: %s\n", getMoveAsString(mouvement_selectionne));

        // Mise à jour de la localisation du rover après le mouvement
        updateLocalisation(&localisation_marc, mouvement_selectionne);

        // Récupération du type de sol actuel
        t_soil sol_actuel = carte.soils[localisation_marc.pos.y][localisation_marc.pos.x];

        // Récupération du coût du sol actuel
        int cout_mouvement = carte.costs[localisation_marc.pos.y][localisation_marc.pos.x];
        cout_total += cout_mouvement; /**< Accumulation du coût total. */
        energie -= cout_mouvement;     /**< Décrémentation de l'énergie. */

        // Affichage de la carte mise à jour avec la nouvelle position du rover
        displayMap(carte, localisation_marc);

        // Affichage du type de sol sur lequel se trouve le rover
        printf("MARC est maintenant sur le type de sol %d\n", sol_actuel);

        // Gestion des conditions spécifiques des types de sol
        switch (sol_actuel) {
            case ERG:
                printf("Attention: Sol ERG, le prochain mouvement coutera le double.\n");
                effet_erg = 1; /**< Activation de l'effet ERG pour le prochain mouvement. */
                break;
            case REG:
                printf("Attention: Sol REG, les options de mouvement sont limiees dans la prochaine phase.\n");
                // Limiter les mouvements disponibles (par exemple, seulement les rotations)
                // Implémentation possible si nécessaire
                break;
            case PENTE:
                printf("Notice: Terrain en pente, les couts de mouvement sont augmentes.\n");
                cout_mouvement += 5; /**< Augmentation du coût du mouvement. */
                break;
            case CREVASSE:
                printf("Danger: MARC est tombe dans une crevasse! Mission echouee.\n");
                mission_completee = 1; /**< Fin de la mission en échec. */
                break;
            case BASE_STATION:
                printf("Félicitations: MARC est arrive à la station de base! Mission accomplie.\n");
                mission_completee = 1; /**< Fin de la mission avec succès. */
                break;
            default:
                break;
        }

        // Vérification si le rover est sur une crevasse ou à la station de base
        if (sol_actuel == CREVASSE || sol_actuel == BASE_STATION) {
            if (sol_actuel == CREVASSE) {
                printf("Mission echouee: MARC est dans une crevasse.\n");
            } else if (sol_actuel == BASE_STATION) {
                printf("Mission reussie: MARC a atteint la station de base.\n");
            }
            printf("Coût total utilisé: %d\n", cout_total);
            freeTree(racine); /**< Libération de la mémoire de l'arbre. */
            break; /**< Sortie de la boucle principale. */
        }

        // Vérification si le rover est sorti des limites de la carte
        if (!isValidLocalisation(localisation_marc.pos, carte.x_max, carte.y_max)) {
            printf("MARC est sorti des limites de la carte à la position (%d, %d)\n",
                   localisation_marc.pos.x, localisation_marc.pos.y);
            printf("Mission echouee: MARC a quitte la carte.\n");
            mission_completee = 1; /**< Fin de la mission. */
            printf("Coût total utilisé avant l'échec: %d\n", cout_total);
            freeTree(racine); /**< Libération de la mémoire de l'arbre. */
            break; /**< Sortie de la boucle principale. */
        }

        // Vérification si l'énergie est épuisée
        if (energie <= 0) {
            printf("MARC a epuise toute son energie! Mission echouee.\n");
            mission_completee = 1; /**< Fin de la mission en échec. */
            printf("Cout total utilise: %d\n", cout_total);
            freeTree(racine); /**< Libération de la mémoire de l'arbre. */
            break; /**< Sortie de la boucle principale. */
        }

        // Affichage de l'énergie restante après le mouvement
        printf("Energie restante: %d\n", energie);

        // Mise à jour du coût de départ pour la prochaine phase
        cout_total = carte.costs[localisation_marc.pos.y][localisation_marc.pos.x];

        // Libération de la mémoire allouée à l'arbre
        freeTree(racine);

        // Incrémentation de la phase
        phase_actuelle++;
    }

    // Libération de la mémoire allouée pour la carte avant de terminer le programme
    for (int i = 0; i < carte.y_max; i++) {
        free(carte.soils[i]);
        free(carte.costs[i]);
    }
    free(carte.soils);
    free(carte.costs);

    return 0; /**< Terminaison du programme avec succès. */
}
