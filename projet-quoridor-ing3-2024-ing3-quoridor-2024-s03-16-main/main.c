    #include <stdio.h>
    #include <stdlib.h>
    #include <windows.h>
    #include <conio.h> // Necessaire pour getch()
    #include <time.h>
    #include <stdbool.h>

    #define UP_ARROW 72
    #define DOWN_ARROW 80
    #define ENTER 13
    #define TAILLE 17


    typedef struct {
        int x;
        int y;
    } t_position;

    typedef struct {
        char nom[40];
        char pion; // Symbole du joueur (ex. 'A', 'B')
        t_position position; // Position actuelle du joueur
        int bar_rest; // Nombre de barrieres restantes
        int score; // Score des parties gagnées
        int deplacements; // Nombre de déplacements effectués
    } t_joueur;


    typedef struct {
        char plateau[TAILLE][TAILLE];
        t_joueur joueurs[4]; // Quatre joueurs max
        int joueur_actuel; // Indique quel joueur joue (0 a 3)
        int nb_joueurs; // Nombre de joueurs (2 ou 4)
    } t_jeu;

    typedef struct {
        char bar_col_1;
        char bar_col_2;
        int bar_lig_1;
        int bar_lig_2;
        char direction;
    } t_barriere;

    void Color(int couleurDuTexte, int couleurDeFond) {
        HANDLE H = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(H, couleurDeFond * 16 + couleurDuTexte);
    }

    int gen_aleatoire(t_jeu *jeu) {
        return rand() % jeu->nb_joueurs;
    }
    void viderTampon() {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }


    void afficher_plateau(char plateau[TAILLE][TAILLE], t_joueur joueurs[], t_jeu *jeu) {
        printf("   Joueur actif : %s  | Jeton : %c  | Murs restants : %d\n\n\n",
               joueurs[jeu->joueur_actuel].nom,
               joueurs[jeu->joueur_actuel].pion,
               joueurs[jeu->joueur_actuel].bar_rest);

        printf("    ");
        for (int i = 0; i < TAILLE; i++) {
            if (i % 2 == 0) {
                printf("%c ", 'A' + i / 2); // Affichage des lettres pour les colonnes
            } else {
                printf("    ");
            }
        }
        printf("\n");

        for (int i = 0; i < TAILLE; i++) {
            if (i % 2 == 0) {
                printf("%2d ", (i / 2) + 1); // Affichage des chiffres pour les lignes
            } else {
                printf("   ");
            }

            for (int j = 0; j < TAILLE; j++) {
                int joueur_present = 0;
                for (int k = 0; k < jeu->nb_joueurs; k++) {
                    if (i == joueurs[k].position.y && j == joueurs[k].position.x) {
                        Color(4 + k, 15); // Differencier les couleurs pour les joueurs
                        printf(" %c ", joueurs[k].pion);
                        Color(15, 0);
                        joueur_present = 1;
                        break;
                    }
                }
                if (!joueur_present) {
                    Color(15, 0);
                    printf(" %c ", plateau[i][j]);
                }
            }
            printf("\n");
        }
    }

    void sauvegarder_partie(t_jeu *jeu) {
        FILE *fichier = fopen("partie.txt", "w");
        if (fichier == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier de sauvegarde.\n");
            return;
        }

        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                fprintf(fichier, "%c", jeu->plateau[i][j]);
            }
            fprintf(fichier, "\n");
        }
        fclose(fichier);
    }

    void reprendre_partie(t_jeu *jeu) {
        char c = 0;
        int resultat;
        FILE *fichier = fopen("partie.txt", "r");
        if (fichier == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier de sauvegarde.\n");
            return;
        }
        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                printf("%c", jeu->plateau[i][j]);

                resultat = fscanf(fichier, "%c", &c);
                if (resultat == EOF) {
                    printf("Erreur de fichier");
                    fclose(fichier);
                    return;
                }
                jeu->plateau[i][j] = c;
            }
        }
        fclose(fichier);
        afficher_plateau(jeu->plateau, jeu->joueurs, jeu);
    }

    void sauvegarder_joueur(t_jeu *jeu) {
        FILE *fichier = fopen("joueurs.txt", "w");
        if (fichier == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier de sauvegarde.\n");
            return;
        }

        for (int i = 0; i < jeu->nb_joueurs; i++) {
            fprintf(fichier, "%s %c %d %d \n",
                    jeu->joueurs[i].nom,
                    jeu->joueurs[i].pion,
                    jeu->joueurs[i].bar_rest,
                    jeu->joueurs[i].score);
        }
        fclose(fichier);
        fichier = NULL;
        printf("Fichier sauvegarde avec succes.\n");
    }

    void reprendre_joueur(t_jeu *jeu, int *nb_joueurs) {
        FILE *fichier = fopen("joueurs.txt", "r");
        if (fichier == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier de sauvegarde.\n");
            return;
        }

        *nb_joueurs = 0; // Initialisation du compteur de joueurs
        while (fscanf(fichier, "%s %c %d %d\n",
                      jeu->joueurs[*nb_joueurs].nom,
                      &jeu->joueurs[*nb_joueurs].pion,
                      &jeu->joueurs[*nb_joueurs].bar_rest,
                      &jeu->joueurs[*nb_joueurs].score) == 4) {
            // Vérifier si le nom du joueur est valide (non vide)
            if (strlen(jeu->joueurs[*nb_joueurs].nom) == 0) {
                printf("Erreur : Nom de joueur vide détecté. Arrêt de la lecture.\n");
                break;
            }
        }
    }

    void sauvegarder_partie_complete(t_jeu *jeu) {
        sauvegarder_partie(jeu->plateau);
        sauvegarder_joueur(jeu);
    }

    void reprise_partie_complete(t_jeu *jeu, int *nb_joueurs) {
        system("cls");
        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                jeu->plateau[i][j] = ' ';
            }
        }
        *nb_joueurs = 0;
        reprendre_partie(jeu);
        reprendre_joueur(jeu, nb_joueurs);
        printf("Reprise partie avec succès\n");
        afficher_plateau(jeu->plateau, jeu->joueurs, jeu);
    }

    void sauvegarder_scores(t_jeu *jeu, const char *fichier) {
        FILE *fp = fopen(fichier, "r+");
        if (fp == NULL) {
            fp = fopen(fichier, "w");
            if (fp == NULL) {
                printf("Erreur : Impossible d'ouvrir le fichier de sauvegarde.\n");
                return;
            }
        }

        // Charger les scores existants dans une structure temporaire
        char noms_existants[100][40];
        int scores_existants[100];
        int nb_scores_existants = 0;

        while (fscanf(fp, "%39s %d", noms_existants[nb_scores_existants],
                      &scores_existants[nb_scores_existants]) == 2) {
            nb_scores_existants++;
        }

        // Mettre à jour les scores des joueurs actuels
        for (int i = 0; i < jeu->nb_joueurs; i++) {
            int joueur_trouve = 0;
            for (int j = 0; j < nb_scores_existants; j++) {
                if (strcmp(jeu->joueurs[i].nom, noms_existants[j]) == 0) {
                    scores_existants[j] = jeu->joueurs[i].score; // Mise à jour du score
                    joueur_trouve = 1;
                    break;
                }
            }
            if (!joueur_trouve) {
                // Ajouter un nouveau joueur si non trouvé
                strcpy(noms_existants[nb_scores_existants], jeu->joueurs[i].nom);
                scores_existants[nb_scores_existants] = jeu->joueurs[i].score;
                nb_scores_existants++;
            }
        }

        // Réécrire tout le fichier
        rewind(fp);
        for (int i = 0; i < nb_scores_existants; i++) {
            fprintf(fp, "%s %d\n", noms_existants[i], scores_existants[i]);
        }

        fclose(fp);
        printf("Scores sauvegardés dans %s.\n", fichier);
    }

    void sauvegarder_historique(t_jeu *jeu, const char *fichier_historique) {
        FILE *fp = fopen(fichier_historique, "a");
        if (fp == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier d'historique.\n");
            return;
        }

        fprintf(fp, "Nouvelle partie:\n");
        for (int i = 0; i < jeu->nb_joueurs; i++) {
            fprintf(fp, "Joueur: %s, Score: %d, Murs restants: %d, Déplacements: %d\n",
                    jeu->joueurs[i].nom,
                    jeu->joueurs[i].score,
                    jeu->joueurs[i].bar_rest,
                    jeu->joueurs[i].deplacements);
        }
        fprintf(fp, "Plateau final:\n");
        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                fprintf(fp, "%c", jeu->plateau[i][j]);
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "-----------------------------\n");

        fclose(fp);
        printf("Historique sauvegardé avec succès.\n");
    }


    bool verifier_victoire(t_jeu *jeu) {
        t_joueur *joueur = &jeu->joueurs[jeu->joueur_actuel];
        jeu->joueurs[jeu->joueur_actuel].score = 0;
        // Vérifie si le joueur actuel a atteint son objectif
        if (jeu->nb_joueurs == 2) {
            if (jeu->joueur_actuel == 0 && joueur->position.y == TAILLE - 1) {
                joueur->score += 5;
                return true;
            } else if (jeu->joueur_actuel == 1 && joueur->position.y == 0) {
                joueur->score += 5;
                return true;
            }
        } else if (jeu->nb_joueurs == 4) {
            if (jeu->joueur_actuel == 0 && joueur->position.y == TAILLE - 1) {
                joueur->score += 5;
                return true;
            } else if (jeu->joueur_actuel == 1 && joueur->position.y == 0) {
                joueur->score += 5;
                return true;
            } else if (jeu->joueur_actuel == 2 && joueur->position.x == TAILLE - 1) {
                joueur->score += 5;
                return true;
            } else if (jeu->joueur_actuel == 3 && joueur->position.x == 0) {
                joueur->score += 5;
                return true;
            }
        }
        printf("Score de %s mis à jour : %d\n", joueur->nom, joueur->score);
        return false;
    }

    void charger_scores(t_jeu *jeu, const char *fichier) {
        FILE *fp = fopen(fichier, "r");
        if (fp == NULL) {
            printf("Aucun fichier de sauvegarde trouvé. Les scores commencent à zéro.\n");
            return;
        }

        char nom[40];
        int score;
        while (fscanf(fp, "%39s %d", nom, &score) == 2) {
            for (int i = 0; i < jeu->nb_joueurs; i++) {
                if (strcmp(nom, jeu->joueurs[i].nom) == 0) {
                    jeu->joueurs[i].score = score; // Associer le score existant
                    break;
                }
            }
        }

        fclose(fp);
        printf("Scores charges depuis %s.\n", fichier);
    }

    void afficher_resume_joueurs(t_jeu *jeu) {
        printf("\nResume des joueurs :\n");
        for (int i = 0; i < jeu->nb_joueurs; i++) {
            printf("Joueur : %s\n", jeu->joueurs[i].nom);
            printf("  - Murs restants : %d\n", jeu->joueurs[i].bar_rest);
            printf("  - Score : %d\n", jeu->joueurs[i].score);
            printf("  - Deplacements effectues : %d\n", jeu->joueurs[i].deplacements);
        }
        printf("\n");
    }


    void initialiser_barrieres(t_joueur *joueurs, int nb_joueurs) {
        int barriere_par_joueur;

        if (nb_joueurs == 2) {
            barriere_par_joueur = 10; // Chaque joueur obtient 10 barrières
        } else if (nb_joueurs == 4) {
            barriere_par_joueur = 5; // Chaque joueur obtient 5 barrières
        } else {
            printf("Erreur : Nombre de joueurs invalide. Seuls 2 ou 4 joueurs sont autorisés.\n");
            return;
        }

        for (int i = 0; i < nb_joueurs; i++) {
            joueurs[i].bar_rest = barriere_par_joueur;
        }
    }

    void initialiser_joueurs(t_joueur *joueur, t_jeu *jeu) {
        do {
            Color(3, 0);
            printf("Entrez le nombre de joueurs (2 ou 4) : ");
            if (scanf("%d", &jeu->nb_joueurs) != 1) {
                printf("Erreur : entree invalide. Veuillez entrer un nombre.\n");
                while (getchar() != '\n'); // Nettoie le tampon
                continue;
            }
            while (getchar() != '\n'); // Nettoie les residus
        } while (jeu->nb_joueurs != 2 && jeu->nb_joueurs != 4);

        for (int i = 0; i < jeu->nb_joueurs; i++) {
            int entree_valide = 0; // Variable pour suivre si l'entrée est valide
            do {
                printf("Entrez le nom du joueur %d : ", i + 1);

                if (scanf("%39s", joueur[i].nom) == 1) {
                    // Si un mot est correctement lu
                    entree_valide = 1; // Marquer l'entrée comme valide
                } else {
                    printf("Erreur : entree invalide. Veuillez entrer un nom.\n");
                    while (getchar() != '\n'); // Nettoie le tampon
                }
            } while (!entree_valide); // Répète jusqu'à une entrée valide

            while (getchar() != '\n'); // Nettoie les résidus du tampon

            printf("Entrez son pion (caractere unique) : ");
            if (scanf(" %c", &joueur[i].pion) != 1) {
                printf("Erreur : entree invalide pour le pion.\n");
                while (getchar() != '\n'); // Nettoie le tampon
                i--; // Refaire l'entree pour ce joueur
                continue;
            }
            while (getchar() != '\n'); // Nettoie le tampon
            initialiser_barrieres(jeu->joueurs, jeu->nb_joueurs);
        }
    }

    void initialiser_positions_joueurs(t_joueur joueurs[], int nb_joueurs) {
        if (nb_joueurs == 2) {
            joueurs[0].position.x = TAILLE / 2;
            joueurs[0].position.y = 0;

            joueurs[1].position.x = TAILLE / 2;
            joueurs[1].position.y = TAILLE - 1;
        } else if (nb_joueurs == 4) {
            joueurs[0].position.x = TAILLE / 2;
            joueurs[0].position.y = 0;

            joueurs[1].position.x = TAILLE / 2;
            joueurs[1].position.y = TAILLE - 1;

            joueurs[2].position.x = 0;
            joueurs[2].position.y = TAILLE / 2;

            joueurs[3].position.x = TAILLE - 1;
            joueurs[3].position.y = TAILLE / 2;
        }
    }

    void initialiser_plateau(char plateau[TAILLE][TAILLE]) {
        for (int i = 0; i < TAILLE; i++) {
            for (int j = 0; j < TAILLE; j++) {
                if (i % 2 == 0 && j % 2 == 0) {
                    plateau[i][j] = '.'; // Cases principales
                } else {
                    plateau[i][j] = ' '; // Espaces pour les barrieres
                }
            }
        }
    }


    void deplacer_pion(t_jeu *jeu) {
        char touche;
        t_joueur *joueur_actuel = &jeu->joueurs[jeu->joueur_actuel];

        printf("Tour de %s (%c).\n", joueur_actuel->nom, joueur_actuel->pion);
        printf("Utilisez Z (haut), S (bas), Q (gauche), D (droite) pour deplacer. Appuyez sur Echap pour quitter.\n");

        while (1) {
            touche = getch();
            t_position nouvelle_position = joueur_actuel->position;
            int deplacement_valide = 1;

            if (toupper(touche) == 'S') {
                // Descendre
                if (nouvelle_position.y + 2 < TAILLE && jeu->plateau[nouvelle_position.y + 1][nouvelle_position.x] !=
                    'B') {
                    nouvelle_position.y += 2;
                    jeu->joueurs[jeu->joueur_actuel].deplacements++;
                } else {
                    printf("Erreur : Deplacement impossible (barriere presente).\n");
                    deplacement_valide = 0;
                }
            } else if (toupper(touche) == 'Z') {
                // Monter
                if (nouvelle_position.y - 2 >= 0 && jeu->plateau[nouvelle_position.y - 1][nouvelle_position.x] != 'B') {
                    nouvelle_position.y -= 2;
                    jeu->joueurs[jeu->joueur_actuel].deplacements++;
                } else {
                    printf("Erreur : Deplacement impossible (barriere presente).\n");
                    deplacement_valide = 0;
                }
            } else if (toupper(touche) == 'D') {
                // Droite
                if (nouvelle_position.x + 2 < TAILLE && jeu->plateau[nouvelle_position.y][nouvelle_position.x + 1] !=
                    'B') {
                    nouvelle_position.x += 2;
                    jeu->joueurs[jeu->joueur_actuel].deplacements++;
                } else {
                    printf("Erreur : Deplacement impossible (barriere presente).\n");
                    deplacement_valide = 0;
                }
            } else if (toupper(touche) == 'Q') {
                // Gauche
                if (nouvelle_position.x - 2 >= 0 && jeu->plateau[nouvelle_position.y][nouvelle_position.x - 1] != 'B') {
                    nouvelle_position.x -= 2;
                    jeu->joueurs[jeu->joueur_actuel].deplacements++;
                } else {
                    printf("Erreur : Deplacement impossible (barriere presente).\n");
                    deplacement_valide = 0;
                }
            } else if (touche == 27) {
                printf("Fin du jeu.\n");
                exit(0);
            } else {
                printf("Touche invalide.\n");
                continue;
            }

            if (deplacement_valide &&
                nouvelle_position.x >= 0 && nouvelle_position.x < TAILLE &&
                nouvelle_position.y >= 0 && nouvelle_position.y < TAILLE &&
                jeu->plateau[nouvelle_position.y][nouvelle_position.x] == '.') {
                joueur_actuel->position = nouvelle_position;

                // Vérification de la victoire après chaque action
                if (verifier_victoire(jeu)) {
                    sauvegarder_scores(jeu, "scores.txt");
                    system("cls");
                    printf("Bravo %s ! Vous avez atteint le cote oppose !\n", jeu->joueurs[jeu->joueur_actuel].nom);
                    afficher_resume_joueurs(jeu);
                }


                jeu->joueur_actuel = (jeu->joueur_actuel + 1) % jeu->nb_joueurs; // Change de joueur
                break;
            } else {
                printf("Deplacement invalide. Reessayez.\n");
            }
        }
    }

    bool placement_barriere(char plateau[TAILLE][TAILLE], t_barriere *bar, t_jeu *jeu) {
        // Demande des coordonnees et de la direction
        printf("Ou souhaitez-vous placer la barriere ? (Exemple: D1 D2 G): ");
        scanf(" %c%d %c%d %c", &bar->bar_col_1, &bar->bar_lig_1, &bar->bar_col_2, &bar->bar_lig_2, &bar->direction);

        // Conversion des coordonnees (Ex: D1 -> colonne = 4, ligne = 0)
        int col1 = (bar->bar_col_1 - 'A') * 2; // Majuscule pour eviter erreurs
        int col2 = (bar->bar_col_2 - 'A') * 2;
        int lig1 = (bar->bar_lig_1 - 1) * 2;
        int lig2 = (bar->bar_lig_2 - 1) * 2;

        // Verification des limites du plateau
        if (col1 < 0 || col1 >= TAILLE || lig1 < 0 || lig1 >= TAILLE ||
            col2 < 0 || col2 >= TAILLE || lig2 < 0 || lig2 >= TAILLE) {
            printf("Erreur : Barriere en dehors des limites du plateau.\n");
            return false;
        }

        // Verification des cases voisines
        if (!(abs(col1 - col2) == 2 && lig1 == lig2) && !(abs(lig1 - lig2) == 2 && col1 == col2)) {
            printf("Erreur : Les cases doivent etre voisines et alignees horizontalement ou verticalement.\n");
            return false;
        }

        // Verification des barrieres restantes pour le joueur
        if (jeu->joueurs[jeu->joueur_actuel].bar_rest <= 0) {
            printf("Erreur : Vous n'avez plus de barrieres restantes !\n");
            return false;
        }

        // Verification si l'emplacement est deja occupe par une autre barriere
        if (plateau[lig1][col1] == 'B' || plateau[lig2][col2] == 'B') {
            printf("Erreur : Une barriere existe deja sur cette position.\n");
            return false;
        }

        // Placement de la barriere
        if (toupper(bar->direction) == 'H') {
            if (lig1 > 0 && plateau[lig1 - 1][col1] == ' ' && plateau[lig2 - 1][col2] == ' ') {
                plateau[lig1 - 1][col1] = 'B';
                plateau[lig2 - 1][col2] = 'B';
            } else {
                printf("Placement impossible en haut.\n");
                return false;
            }
        } else if (toupper(bar->direction) == 'B') {
            if (lig1 < TAILLE - 1 && plateau[lig1 + 1][col1] == ' ' && plateau[lig2 + 1][col2] == ' ') {
                plateau[lig1 + 1][col1] = 'B';
                plateau[lig2 + 1][col2] = 'B';
            } else {
                printf("Erreur : Placement en bas impossible.\n");
                return false;
            }
        } else if (toupper(bar->direction) == 'G') {
            if (col1 > 0 && plateau[lig1][col1 - 1] == ' ' && plateau[lig2][col2 - 1] == ' ') {
                plateau[lig1][col1 - 1] = 'B';
                plateau[lig2][col2 - 1] = 'B';
            } else {
                printf("Erreur : Placement a gauche impossible.\n");
                return false;
            }
        } else if (toupper(bar->direction) == 'D') {
            if (col1 < TAILLE - 1 && plateau[lig1][col1 + 1] == ' ' && plateau[lig2][col2 + 1] == ' ') {
                plateau[lig1][col1 + 1] = 'B';
                plateau[lig2][col2 + 1] = 'B';
            } else {
                printf("Erreur : Placement a droite impossible.\n");
                return false;
            }
        } else {
            printf("Erreur : Direction invalide.\n");
            return false;
        }

        // Mise a jour des barrieres restantes pour le joueur
        jeu->joueurs[jeu->joueur_actuel].bar_rest--;
        printf("Barriere placee avec succes par %s.\n", jeu->joueurs[jeu->joueur_actuel].nom);
        jeu->joueur_actuel = (jeu->joueur_actuel + 1) % jeu->nb_joueurs;
        return true;
    }

    void afficherMenu(int choixActuel) {
        system("cls"); // Efface l'écran (Windows)
        printf("\n===== MENU DU JEU QUORIDOR =====\n");
        printf("\n Utilisez les fleches HAUT/BAS pour naviguer, et ENTER pour selectionner.\n\n");

        char *options[] = {
            "1. Lancer une nouvelle partie",
            "2. Reprendre une partie sauvegardee",
            "3. Afficher l'aide",
            "4. Afficher les scores des joueurs",
            "5. Quitter le jeu"
        };
        int nbOptions = sizeof(options) / sizeof(options[0]);

        for (int i = 0; i < nbOptions; i++) {
            if (i == choixActuel) {
                printf(" > %s <\n", options[i]); // Mise en surbrillance
            } else {
                printf("   %s\n", options[i]);
            }
        }
    }

    void action(t_jeu *jeu) {
        int victoire = 0;

        while (!victoire) {
            afficher_plateau(jeu->plateau, jeu->joueurs, jeu);
            printf("C'est au tour de %s (%c).\n", jeu->joueurs[jeu->joueur_actuel].nom,
                   jeu->joueurs[jeu->joueur_actuel].pion);

            printf("Actions possibles :\n");
            printf("1 - Déplacer le pion\n");
            printf("2 - Poser une barrière\n");
            printf("3 - Passer son tour\n");
            printf("4 - Quitter\n");
            printf("Votre choix : ");

            int choix;
            scanf("%d", &choix);

            switch (choix) {
                case 1:
                    deplacer_pion(jeu);
                    break;

                case 2:
                    t_barriere barriere;
                    if (!placement_barriere(jeu->plateau, &barriere, jeu)) {
                        printf("Action invalide : Impossible de poser une barrière.\n");
                    }
                    break;

                case 3:
                    printf("%s passe son tour.\n", jeu->joueurs[jeu->joueur_actuel].nom);
                    jeu->joueur_actuel = (jeu->joueur_actuel + 1) % jeu->nb_joueurs;
                    break;

                case 4:
                    printf("Voulez-vous sauvegarder la partie avant de la quitter");
                    char reponse;
                    scanf(" %c", &reponse);
                if (toupper(reponse == 'O')) {
                    sauvegarder_partie_complete(jeu);
                    printf("Partie sauvegardée. À bientôt !\n");
                    return;
                }
                if (toupper(reponse == 'N')) {
                    printf("Partie non sauvegardée. À bientôt !\n");
                }

                default:
                    printf("Choix invalide. Veuillez réessayer.\n");
                    break;
            }

            // Vérifier la victoire après chaque action
            if (verifier_victoire(jeu)) {
                victoire = 1;
                printf("Bravo %s ! Vous avez gagné !\n", jeu->joueurs[jeu->joueur_actuel].nom);

                sauvegarder_scores(jeu, "scores.txt");
                sauvegarder_historique(jeu, "historique.txt");

                afficher_resume_joueurs(jeu);
            }
        }
    }


    void afficher_aide() {
        system("cls");
        printf("\n===== AIDE DU JEU =====\n");
        printf("\nLe jeu Quoridor se joue avec un plateau de 9x9 cases.\n");
        printf("Chaque joueur doit atteindre le cote oppose de sa position initiale.\n");
        printf("Vous pouvez vous deplacer ou poser des barrieres pour gener vos adversaires.\n");
        printf("\nAppuyez sur une touche pour revenir au menu...");
        _getch();
    }

    void afficher_scores(const char *fichier) {
        FILE *fp = fopen(fichier, "r");
        if (fp == NULL) {
            printf("Aucun score enregistre pour l'instant.\n");
            printf("\nAppuyez sur une touche pour revenir au menu...");
            _getch();
            return;
        }

        system("cls");
        printf("\n===== SCORES DES JOUEURS =====\n");
        char nom[40];
        int score;
        while (fscanf(fp, "%39s %d", nom, &score) == 2) {
            printf("Joueur: %s | Score: %d\n", nom, score);
        }

        fclose(fp);
        printf("\nAppuyez sur une touche pour revenir au menu...");
        _getch();
    }

    void continuer_partie(t_jeu* jeu) {
        system("cls");
        int victoire = 0;
        printf("Reprise de la partie.\n");

        while (!victoire) {
            system("cls");
            afficher_plateau(jeu->plateau, jeu->joueurs, jeu);
            printf("Choisissez une action.\n");

            int choix;
            scanf("%d", &choix);
            viderTampon();

            switch (choix) {
                case 1:
                    deplacer_pion(jeu);
                    break;

                case 2: {
                    t_barriere barriere;
                    if (!placement_barriere(jeu->plateau, &barriere, jeu)) {
                        printf("Action invalide : Impossible de poser une barrière.\n");
                    }
                    break;
                }
                case 3:
                    jeu->joueur_actuel = (jeu->joueur_actuel + 1) % jeu->nb_joueurs;
                    break;

                case 4:

                    case 5:
                break;
                default:
                    printf("Choix invalide. Veuillez réessayer.\n");
                    break;
            }
        }
    }

    int main() {
        int choixActuel = 0;
        int nbOptions = 5;
        int running = 1;
        int nb_joueurs = 0; // Initialise le nombre de joueurs

        t_jeu *jeu = (t_jeu *)malloc(sizeof(t_jeu));
        if (!jeu) {
            printf("Erreur d'allocation mémoire pour le jeu.\n");
            return 1;
        }
        memset(jeu, 0, sizeof(t_jeu));

        while (running) {
            afficherMenu(choixActuel);
            char touche = _getch();

            switch (touche) {
                case UP_ARROW:
                    choixActuel = (choixActuel - 1 + nbOptions) % nbOptions;
                    break;
                case DOWN_ARROW:
                    choixActuel = (choixActuel + 1) % nbOptions;
                    break;
                case ENTER:
                    system("cls");
                    switch (choixActuel) {
                        case 0:
                            action(&jeu);
                            break;
                        case 1:
                            action(&jeu); // Passez un pointeur vers l'instance de jeu
                            break;
                        case 2 :
                            system("cls");
                            reprise_partie_complete(jeu, &nb_joueurs);
                        if(nb_joueurs < 0) {
                            printf("Impossible\n");
                            system("pause");
                        }
                        break;
                        case 3:
                            afficher_aide();
                            break;
                        case 4:
                            afficher_scores("scores.txt");
                            break;
                        case 5:
                            printf("Quitter le jeu. Merci d'avoir joue !\n");
                            running = 0;
                            break;
                    }
                    break;
            }
        }
    free(jeu);
        return 0;
    }
