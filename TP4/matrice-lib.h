#ifndef MATRICE_H
#define MATRICE_H

#define ERREUR_MEM 1
#define NBDIGITS "1"

typedef struct{

	float **contenu;
	int largeur;
	int hauteur;

} matrice;

/*
	Retourne une matrice de taille spécifiée par les paramètres
	Retourne NULL en cas d'erreur dans les paramètres
	La matrice retournée en paramètre doit être désallouée à la fin de son utilisation
*/	
matrice * creer_matrice(int largeur, int hauteur);

/*
TODO
*/
float acceder_matrice(matrice *mat, int numLigne, int numColonne);

/*
	Affiche une matrice passée en paramètre
	N'affiche rien si le paramètre mat est NULL
*/
void afficher_matrice(matrice *mat);

/*
	Désalloue les chanmps dynamiques d'une matrice passée en paramètre
	Ne fait rien si le paramètre est NULL
*/
void nettoyer_matrice(matrice *mat);

/*
	Affecte le nombre passé en paramètre à la matrice
	i représente la hauteur
	j représente la largeur
	Retourne 0 si pas d'erreurs, -1 sinon
*/
int affecter_matrice(matrice *mat, float nombre, int i, int j);

/*
	Permet à l'utilisateur d'effectuer la saisie de tous les champs d'une matrice donnée
*/
void saisie_matrice(matrice *mat);

/*
	Rend le contenu d'une matrice aléatoire
*/
void randomiser_matrice(matrice *mat, int min, int max);

/*
	Retourne un tableau contenant les valeurs d'une ligne de la matrice passée en paramètre
	Retourne NULL en cas d'erreur
	Le float ** retourné correspond à l'emplacement mémoire de la matrice et doit être free à la fin de son utilisation
*/
float ** ligne_matrice(matrice *mat, int numLigne);

/*
	Retourne un tableau contenant les valeurs d'une colonne de la matrice passée en paramètre
	Retourne NULL en cas d'erreur
	Le float ** retourné correspond à l'emplacement mémoire de la matrice et doit être free à la fin de son utilisation
*/
float ** colonne_matrice(matrice *mat, int numColonne);

/*
	Affiche une colonne de la matrice
	Ne fait rien en cas de mauvais arguments
*/
void afficher_colonne_matrice(matrice *mat, int numColonne);

/*
	Affiche une ligne de la matrice
	Ne fait rien en cas de mauvais arguments
*/
void afficher_ligne_matrice(matrice *mat, int numLigne);

#endif
