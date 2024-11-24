#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "tree.h"

#define MAX_DEPTH 5           /**< Profondeur maximale de l'arbre de mouvements. */
#define MAX_PATH_LENGTH 20    /**< Longueur maximale du chemin trouve. */
#define MAX_ENERGY 100        /**< Energie initiale du rover. */

int main() {
    srand(time(NULL)); /**< Initialisation de la graine pour les fonctions aleatoires. */
    int choix;

    // Affichage des options de cartes disponibles
    printf("Choisissez la carte a charger:\n");
    printf("1. example1.map\n");
    printf("2. training.map\n");
    printf("3. custom1.map\n");
    printf("4. custom2.map\n");
    printf("Entrez votre choix (1, 2, 3, ou 4): ");
    scanf("%d", &choix);

    t_map carte;

    // Chargement de la carte choisie en fonction du systeme d'exploitation (Windows ou autre)
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
        printf("Choix invalide. Chargement de example1.map par defaut.\n");
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
        printf("Choix invalide. Chargement de example1.map par defaut.\n");
        carte = createMapFromFile("../maps/example1.map");
    }
#endif

    // Affichage des dimensions de la carte chargee
    printf("Carte creee avec les dimensions %d x %d\n", carte.y_max, carte.x_max);

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
            printf("%-5d ", carte.costs[i][j]);
        }
        printf("\n");
    }

    // Initialisation de la position du rover
    t_localisation localisation_marc;
    int depart_trouve = 0; /**< Indicateur pour savoir si une position de depart valide est trouvee. */
    int cout_depart = 0;   /**< Cout initial. */
    for (int i = 0; i < carte.y_max && !depart_trouve; i++) {
        for (int j = 0; j < carte.x_max && !depart_trouve; j++) {
            if (carte.soils[i][j] != BASE_STATION && carte.soils[i][j] != CREVASSE) {
                localisation_marc = loc_init(j, i, NORTH); /**< Initialisation de la localisation du rover. */
                cout_depart = carte.costs[i][j];         /**< Cout initial. */
                depart_trouve = 1;                       /**< Position de depart trouvee. */
            }
        }
    }

    // Verification si une position de depart valide a ete trouvee
    if (!depart_trouve) {
        fprintf(stderr, "Erreur: Aucune position de depart valide trouvee.\n");
        // Liberation de la memoire allouee pour la carte
        for (int i = 0; i < carte.y_max; i++) {
            free(carte.soils[i]);
            free(carte.costs[i]);
        }
        free(carte.soils);
        free(carte.costs);
        exit(1); /**< Terminer le programme avec une erreur. */
    }

    // Affichage de la position initiale du rover
    printf("\nPosition initiale de MARC: (%d, %d), orientation: %d (NORTH=0, EAST=1, SOUTH=2, WEST=3)\n",
           localisation_marc.pos.x, localisation_marc.pos.y, localisation_marc.ori);
    displayMap(carte, localisation_marc); /**< Affichage de la carte avec la position du rover. */

    int mission_completee = 0;    /**< Indicateur pour savoir si la mission est terminee. */
    int phase_actuelle = 0;        /**< Compteur des phases. */
    int cout_total = 0;            /**< Cout total cumule. */
    int energie = MAX_ENERGY;      /**< Energie initiale du rover. */
    int effet_erg = 0;             /**< Effet du terrain ERG sur le prochain mouvement. */

    // Definition des couts des mouvements en energie
    static const int _cout_mouvements[9] = {
            10, // F_10
            20, // F_20
            30, // F_30
            10, // B_10
            20, // B_20
            5,  // T_LEFT
            5,  // T_RIGHT
            10, // U_TURN
            15  // L_90
    };

    // Boucle principale de la mission
    while (!mission_completee) {
        printf("\n--- Phase %d ---\n", phase_actuelle + 1);

        // Affichage de l'energie restante
        printf("Energie restante: %d\n", energie);

        // Demande a l'utilisateur de selectionner le nombre de mouvements disponibles pour la phase suivante
        int nb_mouvements_disponibles = 0;
        printf("Entrez le nombre de mouvements disponibles pour la prochaine phase (jusqu'a 9): ");
        scanf("%d", &nb_mouvements_disponibles);

        // Validation du nombre de mouvements selectionnes
        if (nb_mouvements_disponibles < 1 || nb_mouvements_disponibles > 9) {
            printf("Nombre de mouvements invalide. Defaut a 5 mouvements.\n");
            nb_mouvements_disponibles = 5;
        }

        // Affichage des mouvements disponibles pour la selection
        printf("Selectionnez %d mouvements dans la liste ci-dessous en entrant leurs numeros separes par des espaces:\n", nb_mouvements_disponibles);
        for (int i = 0; i < 9; i++) {
            printf("%d. %s\n", i + 1, getMoveAsString(i));
        }

        int indices_mouvements_selectionnes[9]; /**< Indices des mouvements selectionnes. */
        t_move mouvements_disponibles[9];      /**< Mouvements disponibles pour cette phase. */
        for (int i = 0; i < nb_mouvements_disponibles; i++) {
            scanf("%d", &indices_mouvements_selectionnes[i]); /**< Lecture des choix de l'utilisateur. */
            if (indices_mouvements_selectionnes[i] < 1 || indices_mouvements_selectionnes[i] > 9) {
                printf("Numero de mouvement invalide. Veuillez entrer un numero entre 1 et 9.\n");
                i--; /**< Redemander ce mouvement. */
            } else {
                mouvements_disponibles[i] = indices_mouvements_selectionnes[i] - 1; /**< Conversion en indice de l'enumeration. */
            }
        }

        // Mesure du temps de construction de l'arbre des mouvements
        clock_t debut_construction = clock();
        t_treeNode *racine = createTreeNode(localisation_marc, cout_depart, -1, NULL); /**< Creation de la racine de l'arbre. */
        buildTree(racine, 0, MAX_DEPTH, mouvements_disponibles, nb_mouvements_disponibles, carte); /**< Construction de l'arbre. */
        clock_t fin_construction = clock();
        double temps_construction = ((double)(fin_construction - debut_construction)) / CLOCKS_PER_SEC; /**< Calcul du temps ecoule. */

        // Mesure du temps de recherche de la feuille avec le cout minimal
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

        // Affichage des metriques de performance
        printf("\n--- Performance Metrics ---\n");
        printf("Temps de construction de l'arbre: %.6f secondes\n", temps_construction);
        printf("Temps de recherche de la feuille minimale: %.6f secondes\n", temps_recherche);
        printf("Temps de calcul du chemin: %.6f secondes\n", temps_chemin);
        printf("---------------------------\n");

        // Affichage du chemin optimal trouve
        printf("\nChemin optimal trouve avec un cout total de %d:\n", feuille_min->cost);
        for (int i = 0; i < longueur_chemin; i++) {
            printf("Etape %d: Mouvement %s\n", i + 1, getMoveAsString(chemin[i]));
        }

        // Application du premier mouvement du chemin optimal
        t_move mouvement_selectionne = chemin[0];

        printf("\nExecution du mouvement: %s\n", getMoveAsString(mouvement_selectionne));

        // Recuperation du cout du mouvement selectionne
        int cout_mouvement = _cout_mouvements[mouvement_selectionne];

        // Application de l'effet du terrain ERG si actif
        if (effet_erg) {
            cout_mouvement *= 2; /**< Double le cout du mouvement. */
            effet_erg = 0;       /**< Reinitialisation de l'effet. */
        }

        // Mise a jour de la localisation du rover apres le mouvement
        updateLocalisation(&localisation_marc, mouvement_selectionne);

        // Verification si le rover est sorti des limites de la carte
        int hors_limites = 0;
        if (!isValidLocalisation(localisation_marc.pos, carte.x_max, carte.y_max)) {
            printf("MARC a quitte les limites de la carte a la position (%d, %d)\n",
                   localisation_marc.pos.x, localisation_marc.pos.y);
            printf("Mission echouee: MARC a quitte la carte.\n");
            mission_completee = 1; /**< Fin de la mission. */
            printf("Energie totale utilisee avant l'echec: %d\n", MAX_ENERGY - energie);
            freeTree(racine); /**< Liberation de la memoire de l'arbre. */
            break; /**< Sortie de la boucle principale. */
        }

        // Affichage de la carte mise a jour avec la nouvelle position du rover
        displayMap(carte, localisation_marc);

        // Recuperation du type de sol actuel
        t_soil sol_actuel = carte.soils[localisation_marc.pos.y][localisation_marc.pos.x];

        // Affichage du type de sol sur lequel se trouve le rover
        printf("MARC est maintenant sur le type de sol %d\n", sol_actuel);

        // Gestion des conditions specifiques des types de sol
        switch (sol_actuel) {
            case ERG:
                printf("Avertissement: Sol ERG, le prochain mouvement coutera double.\n");
                effet_erg = 1; /**< Activation de l'effet ERG pour le prochain mouvement. */
                break;
            case REG:
                printf("Avertissement: Sol REG, options de mouvements limities dans la prochaine phase.\n");
                // Limiter les mouvements disponibles (par exemple, seulement les rotations)
                // Vous pouvez implementer cela si necessaire
                break;
            case PENTE:
                printf("Notification: Sol en pente, les couts des mouvements sont augmentes.\n");
                cout_mouvement += 5; /**< Augmentation du cout du mouvement. */
                break;
            case CREVASSE:
                printf("Danger: MARC est tombe dans une crevasse! Mission echouee.\n");
                mission_completee = 1; /**< Fin de la mission en echec. */
                break;
            case BASE_STATION:
                printf("Felicitation: MARC est arrive a la base! Mission accomplie.\n");
                mission_completee = 1; /**< Fin de la mission avec succes. */
                break;
            default:
                break;
        }

        // Verification si le rover est sur une crevasse ou a la station de base
        if (sol_actuel == CREVASSE || sol_actuel == BASE_STATION) {
            if (sol_actuel == CREVASSE) {
                printf("Mission echouee: MARC est dans une crevasse.\n");
            } else if (sol_actuel == BASE_STATION) {
                printf("Mission reussie: MARC a atteint la base.\n");
            }
            printf("Energie totale utilisee: %d\n", MAX_ENERGY - energie);
            freeTree(racine); /**< Liberation de la memoire de l'arbre. */
            break; /**< Sortie de la boucle principale. */
        }

        // Decrementation de l'energie en fonction du cout du mouvement
        energie -= cout_mouvement;

        // Verification si l'energie est epuisee
        if (energie <= 0) {
            printf("MARC a epuis les energies! Mission echouee.\n");
            mission_completee = 1; /**< Fin de la mission en echec. */
            printf("Energie totale utilisee: %d\n", MAX_ENERGY - energie);
            freeTree(racine); /**< Liberation de la memoire de l'arbre. */
            break; /**< Sortie de la boucle principale. */
        }

        // Affichage de l'energie restante apres le mouvement
        printf("Energie restante: %d\n", energie);

        // Mise a jour du cout de depart pour la prochaine phase
        cout_depart = carte.costs[localisation_marc.pos.y][localisation_marc.pos.x];

        // Liberation de la memoire allouee a l'arbre
        freeTree(racine);

        // Incrementation de la phase
        phase_actuelle++;
    }

    // Liberation de la memoire allouee pour la carte avant de terminer le programme
    for (int i = 0; i < carte.y_max; i++) {
        free(carte.soils[i]);
        free(carte.costs[i]);
    }
    free(carte.soils);
    free(carte.costs);

    return 0;
}
