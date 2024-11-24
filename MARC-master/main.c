#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "tree.h"

#define MAX_DEPTH 3
#define MAX_PATH_LENGTH 20
#define MAX_ENERGY 500

int main() {
    srand(time(NULL)); // Initialisation du generateur de nombres aleatoires.
    int choix;

    // Affichage des options de cartes disponibles
    printf("Choisissez la carte a charger:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("3. custom1.map\n");
    printf("4. custom2.map\n");
    printf("Entrez votre choix (1, 2, 3 ou 4): ");
    scanf("%d", &choix);

    t_map carte;

#if defined(_WIN32) || defined(_WIN64)
    // Chargement de la carte selectionnee pour les systemes Windows
    if (choix == 1) {
        carte = createMapFromFile("..\\maps\\example1.map");
    } else if (choix == 2) {
        carte = createTrainingMap();
    } else if (choix == 3) {
        carte = createMapFromFile("..\\maps\\custom1.map");
    } else if (choix == 4) {
        carte = createMapFromFile("..\\maps\\custom2.map");
    } else {
        printf("Choix invalide. Chargement de example1.map par defaut.\n");
        carte = createMapFromFile("..\\maps\\example1.map");
    }
#else
    // Chargement de la carte selectionnee pour les systemes Unix/Linux
    if (choix == 1) {
        carte = createMapFromFile("../maps/example1.map");
    } else if (choix == 2) {
        carte = createTrainingMap();
    } else if (choix == 3) {
        carte = createMapFromFile("../maps/custom1.map");
    } else if (choix == 4) {
        carte = createMapFromFile("../maps/custom2.map");
    } else {
        printf("Choix invalide. Chargement de example1.map par defaut.\n");
        carte = createMapFromFile("../maps/example1.map");
    }
#endif

    // Affichage des dimensions de la carte chargee
    printf("Carte creee avec des dimensions %d x %d\n", carte.y_max, carte.x_max);

    // Affichage des types de sols sur la carte
    printf("Affichage des types de sols:\n");
    for (int i = 0; i < carte.y_max; i++) {
        for (int j = 0; j < carte.x_max; j++) {
            printf("%d ", carte.soils[i][j]);
        }
        printf("\n");
    }

    // Affichage des couts des terrains sur la carte
    printf("\nAffichage des couts:\n");
    for (int i = 0; i < carte.y_max; i++) {
        for (int j = 0; j < carte.x_max; j++) {
            printf("%-5d ", carte.costs[i][j]); // Affiche les couts avec un alignement
        }
        printf("\n");
    }

    t_localisation localisation_marc;
    int depart_trouve = 0;
    int cout_total = 0;
    int energie = MAX_ENERGY;
    int effet_erg = 0;

    // Recherche d'une position de depart valide pour MARC
    for (int i = 0; i < carte.y_max && !depart_trouve; i++) {
        for (int j = 0; j < carte.x_max && !depart_trouve; j++) {
            if (carte.soils[i][j] != BASE_STATION && carte.soils[i][j] != CREVASSE) {
                localisation_marc = loc_init(j, i, NORTH);          // Initialisation de la localisation de MARC
                cout_total = carte.costs[i][j];                     // Initialisation du cout total
                energie -= carte.costs[i][j];                       // Deduction de l'energie utilisee pour le depart
                depart_trouve = 1;                                   // Indicateur de depart trouve
            }
        }
    }

    // Verification si une position de depart valide a ete trouvee
    if (!depart_trouve) {
        fprintf(stderr, "Erreur: Aucune position de depart valide trouvee.\n");
        // Liberation de la memoire allouee a la carte
        for (int i = 0; i < carte.y_max; i++) {
            free(carte.soils[i]);
            free(carte.costs[i]);
        }
        free(carte.soils);
        free(carte.costs);
        exit(1); // Terminer le programme en cas d'erreur
    }

    // Affichage de la position initiale de MARC
    printf("\nPosition initiale de MARC: (%d, %d), orientation: %d (NORD=0, EST=1, SUD=2, OUEST=3)\n",
           localisation_marc.pos.x, localisation_marc.pos.y, localisation_marc.ori);
    displayMap(carte, localisation_marc); // Affichage de la carte avec la position de MARC

    int mission_completee = 0;
    int phase_actuelle = 0;

    // Boucle principale du programme, executant les phases de la mission
    while (!mission_completee) {
        printf("\n--- Phase %d ---\n", phase_actuelle + 1);
        printf("Energie restante: %d\n", energie);

        int nb_mouvements_disponibles = 0;
        printf("Entrez le nombre de mouvements disponibles pour la prochaine phase (jusqua 9): ");
        scanf("%d", &nb_mouvements_disponibles);

        // Validation du nombre de mouvements disponibles
        if (nb_mouvements_disponibles < 1 || nb_mouvements_disponibles > 9) {
            printf("Nombre de mouvements invalide. Defaut a 5 mouvements.\n");
            nb_mouvements_disponibles = 5;
        }

        // Selection des mouvements disponibles par l'utilisateur
        printf("Selectionnez %d mouvements dans la liste ci-dessous en entrant leurs numeros separes par des espaces:\n", nb_mouvements_disponibles);
        for (int i = 0; i < 9; i++) {
            printf("%d. %s\n", i + 1, getMoveAsString(i));
        }

        int indices_mouvements_selectionnes[9];
        t_move mouvements_disponibles[9];
        for (int i = 0; i < nb_mouvements_disponibles; i++) {
            scanf("%d", &indices_mouvements_selectionnes[i]);
            // Validation des indices de mouvements selectionnes
            if (indices_mouvements_selectionnes[i] < 1 || indices_mouvements_selectionnes[i] > 9) {
                printf("Numero de mouvement invalide. Veuillez entrer un numero entre 1 et 9.\n");
                i--; // Refaire la selection pour ce mouvement
            } else {
                mouvements_disponibles[i] = indices_mouvements_selectionnes[i] - 1; // Ajustement de l'indice
            }
        }

        // Construction de l'arbre de mouvements
        clock_t debut_construction = clock();
        t_treeNode *racine = createTreeNode(localisation_marc, cout_total, -1, NULL); // Creation de la racine de l'arbre
        buildTree(racine, 0, MAX_DEPTH, mouvements_disponibles, nb_mouvements_disponibles, carte); // Construction recursive
        clock_t fin_construction = clock();
        double temps_construction = ((double)(fin_construction - debut_construction)) / CLOCKS_PER_SEC; // Calcul du temps de construction

        // Recherche de la feuille avec le cout minimal
        clock_t debut_recherche = clock();
        t_treeNode *feuille_min = findMinCostLeaf(racine, NULL);
        clock_t fin_recherche = clock();
        double temps_recherche = ((double)(fin_recherche - debut_recherche)) / CLOCKS_PER_SEC; // Calcul du temps de recherche

        // Reconstruction du chemin optimal depuis la feuille minimale
        clock_t debut_chemin = clock();
        t_move chemin[MAX_PATH_LENGTH];
        int longueur_chemin = 0;
        getPathFromLeaf(feuille_min, chemin, &longueur_chemin);
        clock_t fin_chemin = clock();
        double temps_chemin = ((double)(fin_chemin - debut_chemin)) / CLOCKS_PER_SEC; // Calcul du temps de reconstruction du chemin

        // Affichage des metriques de performance
        printf("\n--- Metriques de Performance ---\n");
        printf("Temps de construction de l'arbre: %.6f secondes\n", temps_construction);
        printf("Temps de recherche de la feuille minimale: %.6f secondes\n", temps_recherche);
        printf("Temps de calcul du chemin: %.6f secondes\n", temps_chemin);
        printf("-------------------------------\n");

        // Affichage du chemin optimal trouve
        printf("\nChemin optimal trouve avec un cout total de %d:\n", feuille_min->cost);
        for (int i = 0; i < longueur_chemin; i++) {
            printf("Etape %d: Mouvement %s\n", i + 1, getMoveAsString(chemin[i]));
        }

        // Verifier si aucun mouvement n'est disponible
        if (longueur_chemin == 0) {
            printf("Aucun mouvement disponible. Mission terminee.\n");
            mission_completee = 1;
            freeTree(racine); // Liberation de l'arbre
            break;
        }

        // Execution du premier mouvement du chemin optimal
        t_move mouvement_selectionne = chemin[0];
        printf("\nExecution du mouvement: %s\n", getMoveAsString(mouvement_selectionne));
        updateLocalisation(&localisation_marc, mouvement_selectionne); // Mise a jour de la localisation de MARC

        // Verification immediat de la validite de la nouvelle position
        if (!isValidLocalisation(localisation_marc.pos, carte.x_max, carte.y_max)) {
            printf("MARC est sorti des limites de la carte a la position (%d, %d)\n",
                   localisation_marc.pos.x, localisation_marc.pos.y);
            printf("Mission echouee: MARC a quitte la carte.\n");
            mission_completee = 1;
            printf("Cout total utilise avant l'echec: %d\n", cout_total);
            freeTree(racine); // Liberation de l'arbre
            break;
        }

        // Acces securise aux donnees de la carte apres verification
        t_soil sol_actuel = carte.soils[localisation_marc.pos.y][localisation_marc.pos.x];
        int cout_mouvement = carte.costs[localisation_marc.pos.y][localisation_marc.pos.x];
        cout_total += cout_mouvement; // Mise a jour du cout total
        energie -= cout_mouvement;     // Deduction de l'energie utilisee
        displayMap(carte, localisation_marc); // Affichage de la carte avec la nouvelle position de MARC
        printf("MARC est maintenant sur le type de sol %d\n", sol_actuel);

        // Gestion des effets specifiques des types de sols
        switch (sol_actuel) {
            case ERG:
                printf("Attention: Sol ERG, le prochain mouvement coutera le double.\n");
                effet_erg = 1; // Indicateur d'effet ERG actif
                break;
            case REG:
                printf("Attention: Sol REG, les options de mouvement sont limiees dans la prochaine phase.\n");
                break;
            case PENTE:
                printf("Notice: Terrain en pente, les couts de mouvement sont augmentes.\n");
                cout_mouvement += 5; // Augmentation du cout de mouvement sur terrain en pente
                break;
            case CREVASSE:
                printf("Danger: MARC est tombe dans une crevasse! Mission echouee.\n");
                mission_completee = 1; // Terminer la mission en cas de crevasse
                break;
            case BASE_STATION:
                printf("Felicitation: MARC est arrive a la station de base! Mission accomplie.\n");
                mission_completee = 1; // Terminer la mission en cas d'arrivee a la station de base
                break;
            default:
                break;
        }

        // Verification des conditions de terminaison de la mission
        if (sol_actuel == CREVASSE || sol_actuel == BASE_STATION) {
            if (sol_actuel == CREVASSE) {
                printf("Mission echouee: MARC est dans une crevasse.\n");
            } else if (sol_actuel == BASE_STATION) {
                printf("Mission reussie: MARC a atteint la station de base.\n");
            }
            printf("Cout total utilise: %d\n", cout_total);
            freeTree(racine); // Liberation de l'arbre
            break;
        }

        // Verification si l'energie de MARC est epuisee
        if (energie <= 0) {
            printf("MARC a epuiser toute son energie! Mission echouee.\n");
            mission_completee = 1;
            printf("Cout total utilise: %d\n", cout_total);
            freeTree(racine); // Liberation de l'arbre
            break;
        }

        printf("Energie restante: %d\n", energie); // Affichage de l'energie restante
        cout_total = carte.costs[localisation_marc.pos.y][localisation_marc.pos.x]; // Mise a jour du cout total
        freeTree(racine); // Liberation de l'arbre
        phase_actuelle++;  // Passage a la phase suivante
    }

    // Liberation de la memoire allouee a la carte
    for (int i = 0; i < carte.y_max; i++) {
        free(carte.soils[i]);
        free(carte.costs[i]);
    }
    free(carte.soils);
    free(carte.costs);

    return 0; // Fin du programme
}
