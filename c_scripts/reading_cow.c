#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* #include <unistd.h> */
#include <time.h>

// macros utiles (KEEP IT SIMPLE STUPID!!!!)
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define CLAMP(x, min, max) MAX(min, MIN(x, max))
#define UPDATE printf("\033[H\033[J")
#define GOTOXY(x, y) printf("\033[%d;%dH", y, x)

// États de l'automate qui parse les arguments de la ligne de commande
#define LIRE_OPTION 0
#define YEUX 1
#define LANGUE 2
#define QUEUE 3
#define MESSAGE 4
#define BOX_WIDTH 5
#define TYPE_ALLIGNEMENT 6

// États de l'allignement vertical
#define ALLIGNE_GAUCHE 0
#define TEXTE_CENTRE   1
#define ALLIGNE_DROITE 2


int est_pair(int n) {  // comment ça % existe ?
	if (n == 0) {
		return 0;
	} else if (n == 1) {
		return 1;
	} else if (n < 0) {
		return (est_pair(-n));
	} else {
		return (est_pair(n - 2));
	}
}


void wait(int duree) {  // duree en milisecondes
	int temps_ecoule = 0;
	clock_t temps_precedent = clock();
	while (temps_ecoule < duree) {
		clock_t delta_time = clock() - temps_precedent;
		temps_ecoule = delta_time * 1000 / CLOCKS_PER_SEC;
	}
}


char* genere_queue(int longueur) {
	char* queue = NULL;
	queue = (char *) malloc((sizeof (char)) * longueur);
	for (int i=0; i < longueur; i++) {
		if (est_pair(i)) {
			queue[i] = '/';
		} else {
			queue[i] = '\\';
		}
	}
	return queue;
}


void affiche_vache(char* yeux, char* langue, int longueur_queue, int pos_x, int pos_y) {
	char* queue = genere_queue(longueur_queue);
	char* tableau_vache[11] = {
		"\\   ^__^\n",
        " \\  (",
		yeux,
		")\\_______\n",
        "    (__)\\       )",
		queue,
		"\n",
        "     ",
		langue,
		" ||----w |\n",
        "        ||     ||\n"
	};

	GOTOXY(pos_x, pos_y);
	for (int i=0; i<11; i++) {
		printf("%s", tableau_vache[i]);
		if (tableau_vache[i][strlen(tableau_vache[i] + 1)] == '\n') {
			pos_y++;
			GOTOXY(pos_x, pos_y);
		}
	}

	free(queue);
}


int mot_plus_long(char* texte) {
	int max_longueur = 0;
	int longueur_mot = 0;
	for (int i=0; texte[i] != '\0'; i++) {
		if (texte[i] == ' ') {
			if (longueur_mot > max_longueur) {
				max_longueur = longueur_mot;
			}
			longueur_mot = 0;
		} else {
			longueur_mot++;
		}
	}
	return max_longueur;
}


int longueur_mot_suivant(char* texte) {
	int longueur = 0;
	for (int i=0; texte[i] != ' ' && texte[i] != '\0'; i++) {
		longueur++;
	}
	return longueur;
}


int nb_lignes_boite(char* texte, int max_longueur_ligne) {
	int nb_lignes = 1;
	int longueur_ligne = 0;
	int mot_suivant;
	/* printf("=>>————<>————-—-—--· ·\n"); */
	while (strlen(texte) > 0) {
		mot_suivant = longueur_mot_suivant(texte);
		if (longueur_ligne + mot_suivant >= max_longueur_ligne) {
			nb_lignes++;
			longueur_ligne = mot_suivant;
			/* printf("—<==>—\n"); */
		} else {
			longueur_ligne += mot_suivant + 1;  // +1 pour l'espace
		}

		/* for (int i=0; i<mot_suivant; i++) { */
		/* 	printf("%c", texte[i]); */
		/* } */
		/* printf("\n"); */

		texte += sizeof(char) * (mot_suivant + 1);  // +1 pour l'espace
	}
	/* printf("=>>————<>————-—-—--· ·\n"); */
	return nb_lignes;
}


char* extraire_ligne(char* texte, int max_longueur_ligne) {
	char* rv_ligne = malloc(sizeof(char) * max_longueur_ligne);
	int longueur_ligne = 0;
	int mot_suivant = longueur_mot_suivant(texte);
	while (longueur_ligne + mot_suivant <= max_longueur_ligne) {
		longueur_ligne += mot_suivant + 1;  // +1 pour l'espace
		mot_suivant = longueur_mot_suivant(texte + (sizeof(char) * longueur_ligne));
	}

	// -1 pour virer le dernier espace de la ligne
	for (int i=0; i<longueur_ligne - 1; i++) {
		rv_ligne[i] = texte[i];

		// finir proprement la dernière ligne
		if (strlen(texte + (sizeof(char) * i)) == 1) {
			rv_ligne[i] = '\0';
			break;
		}
	}

	/* printf("%d: %s|\n", longueur_ligne, rv_ligne); */
	return rv_ligne;
}


void allignement_vertical(char* ligne, int max_longueur_ligne, int allignemet) {
	char* espaces = malloc(sizeof(char) * max_longueur_ligne);
	int nb_caracteres_ligne = strlen(ligne);
	int nb_espaces = max_longueur_ligne - nb_caracteres_ligne;

	switch (allignemet) {
		case ALLIGNE_GAUCHE:
			for (int i=nb_caracteres_ligne; i<max_longueur_ligne; i++) {
				ligne[i] = ' ';
			}
			break;

		case TEXTE_CENTRE:
			for (int i=0; i<(nb_espaces / 2) + (nb_espaces % 2); i++) {
				espaces[i] = ' ';
			}
			strcat(espaces, ligne);
			for (int i=nb_caracteres_ligne + (nb_espaces / 2); i<max_longueur_ligne; i++) {
				espaces[i] = ' ';
			}
			strcpy(ligne, espaces);
			break;

		case ALLIGNE_DROITE:
			for (int i=0; i<nb_espaces; i++) {
				espaces[i] = ' ';
			}
			strcat(espaces, ligne);
			strcpy(ligne, espaces);
			break;
	}

	free(espaces);
}


char** texte_dans_boite(char* texte, int nb_lignes, int max_longueur_lignes, int allignement) {
	char** rv_texte_formatte = malloc(sizeof(char*) * nb_lignes);
	for (int i=0; i<nb_lignes; i++) {
		rv_texte_formatte[i] = extraire_ligne(texte, max_longueur_lignes);
		texte += sizeof(char) * (strlen(rv_texte_formatte[i]) + 1);  // +1 pour virer l'espace
		allignement_vertical(rv_texte_formatte[i], max_longueur_lignes, allignement);
	}
	return rv_texte_formatte;
}


int largeur_boite(char* texte, int longueur_modifie, int longueur_defaut) {
	int longueur_texte = strlen(texte);
	int mot_long_max = mot_plus_long(texte);
	int largeur_boite_finale;
	if (longueur_modifie == 1) {
		largeur_boite_finale = MAX(longueur_defaut, mot_long_max);
	} else {
		largeur_boite_finale = CLAMP(longueur_texte, mot_long_max, longueur_defaut);
	}

	return largeur_boite_finale;
}


void affiche_boite(char** texte_par_lignes, int nb_lignes) {
	int longueur_lignes = strlen(texte_par_lignes[0]);

	printf("+");
	for (int i=0; i<longueur_lignes; i++) {
		printf("—");
	}
	printf("+\n");

	for (int i=0; i<nb_lignes; i++) {
		printf("|%s|\n", texte_par_lignes[i]);
	}

	printf("+");
	for (int i=0; i<longueur_lignes; i++) {
		printf("—");
	}
	printf("+\n");
}


int main(int argc, char* argv[]) {
	char message[250];
	char yeux[3] = "OO\0";
	char langue[3] = "  \0";
	int longueur_queue = 3;
	int largeur_boite_defaut = 40;
	int largeur_boite_modifie = 0;
	int allignement = ALLIGNE_GAUCHE;

	int etat_courant = LIRE_OPTION;
	int etat_suivant;

	// Parser les arguments de la ligne de commande
	for (int i=1; i < argc; i++) {
		switch (etat_courant) {
			case LIRE_OPTION:
				if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--eyes") == 0) {
					etat_suivant = YEUX;
				} else if (strcmp(argv[i], "-T") == 0) {
					etat_suivant = LANGUE;
				} else if (strcmp(argv[i], "--tail") == 0) {
					etat_suivant = QUEUE;
				} else if ((strcmp(argv[i], "-w") == 0) ||
						(strcmp(argv[i], "--width") == 0)) {
							etat_suivant = BOX_WIDTH;
				} else if (strcmp(argv[i], "-a") == 0 ||
						(strcmp(argv[i], "--allign") == 0)) {
					etat_suivant = TYPE_ALLIGNEMENT;
				} else {  // aucune option trouvé => message à afficher
					etat_suivant = MESSAGE;
					strcpy(message, argv[i]);
					strcat(message, " ");
				}
				break;

			case YEUX:
				strcpy(yeux, argv[i]);
				etat_suivant = LIRE_OPTION;
				break;

			case LANGUE:
				strcpy(langue, argv[i]);
				etat_suivant = LIRE_OPTION;
				break;

			case QUEUE:
				sscanf(argv[i], "%i", &longueur_queue);
				etat_suivant = LIRE_OPTION;
				break;

			case BOX_WIDTH:
				sscanf(argv[i], "%i", &largeur_boite_defaut);
				largeur_boite_modifie = 1;
				etat_suivant = LIRE_OPTION;
				break;

			case TYPE_ALLIGNEMENT:
				if (strcmp(argv[i], "gauche") == 0) {
					allignement = ALLIGNE_GAUCHE;
				} else if (strcmp(argv[i], "droite") == 0) {
					allignement = ALLIGNE_DROITE;
				} else if (strcmp(argv[i], "centre") == 0) {
					allignement = TEXTE_CENTRE;
				} else {
					printf("Allignement vertical non valide.\n");
					return 1;
				}
				etat_suivant = LIRE_OPTION;
				break;

			// On ne peut pas mettre d'autres arguments après avoir commencé le message
			case MESSAGE:
				strcat(message, argv[i]);
				strcat(message, " ");
				break;
		}
		etat_courant = etat_suivant;
	}

	int longueur_lignes = largeur_boite(message, largeur_boite_modifie, largeur_boite_defaut);
	int nb_lignes = nb_lignes_boite(message, longueur_lignes);

	char** texte_formate = texte_dans_boite(message, nb_lignes, longueur_lignes, allignement);
	affiche_boite(texte_formate, nb_lignes);

	for (int i=0; i<nb_lignes; i++) {
		free(texte_formate[i]);
	}
	free(texte_formate);

	return 0;
}
