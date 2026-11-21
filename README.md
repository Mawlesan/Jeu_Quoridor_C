# Quoridor en C – Jeu de stratégie en console

Implémentation en **langage C** du jeu de société **Quoridor**, jouable dans la console Windows à 2 ou 4 joueurs.
Projet d'équipe réalisé en 3e année du cycle ingénieur (ING3) à l'**ECE Paris**, 2024.

## Le jeu

Chaque joueur doit être le premier à atteindre le bord opposé du plateau (9 × 9 cases).
À chaque tour, il peut **déplacer son pion** d'une case ou **poser une barrière** pour ralentir ses adversaires.

## Fonctionnalités

- Menu navigable avec les flèches du clavier : nouvelle partie, reprise, aide, scores, quitter
- Parties à **2 ou 4 joueurs**, avec nom et pion personnalisés
- Plateau 9 × 9 affiché en console, pions en couleur
- Déplacement des pions au clavier (Z, Q, S, D)
- Pose de barrières par coordonnées (ex : D1 D2 G)
- Possibilité de passer son tour
- **Sauvegarde et reprise** d'une partie (fichier)
- **Scores** et **historique des parties** enregistrés dans des fichiers texte

## Compiler et lancer

Le jeu fonctionne sous **Windows** (il utilise windows.h).

Commandes : 

```bash
gcc *.c -o quoridor.exe
quoridor.exe
```

## Ma contribution

- **Placement des barrières** :
  - Plateau modélisé par une matrice 17×17 (indices impairs réservés aux murs), avec conversion des coordonnées saisies (ex. `D1 D2 G`)
  - Contrôles avant placement (limites, adjacence des cases, stock restant, collisions), puis écriture du mur et passage au joueur suivant

- **Robustesse et respect des règles** :
  - Validation des saisies utilisateur (vérification des retours de `scanf`, nettoyage du tampon d'entrée, nouvelle demande en cas d'erreur)
  - Rejet des coups illégaux (sortie du plateau, déplacement à travers un mur, barrière invalide), le joueur rejouant son tour

## Équipe

Projet réalisé en équipe de 4 étudiants.
