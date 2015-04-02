#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <signal.h>
#include "matrice-lib.h"

/*Affiche les emplacement d'une matrice de manière simplifiée pour l'utilisateur */
void modele_matrice(matrice *mat);

matrice * creer_matrice(int largeur, int hauteur){
	
	matrice *mat = NULL;
	int i;
	
	//Précond
	if(largeur <= 0 || hauteur <= 0){
	
		fprintf(stderr, "[creer_matrice] Mauvais arguments");
		return NULL;
	}
	
	//Allocation de tous les champs dynamiques
	if((mat = (matrice *)malloc(sizeof(matrice))) == NULL){
	
		perror("Erreur dans l'allocation d'une matrice");
		exit(ERREUR_MEM);
	}
	
	if((mat->contenu = (float **)malloc(sizeof(float *) * hauteur)) == NULL){
	
		perror("Erreur dans l'allocation du contenu d'une matrice");
		exit(ERREUR_MEM);
	}
	
	for(i=0; i<hauteur; i++){
	
		if((mat->contenu[i] = (float *)malloc(sizeof(float)*largeur)) == NULL){
		
			perror("Erreur dans l'allocation d'une ligne d'une matrice");
			exit(ERREUR_MEM);
		}
	}
	
	//Initialisation l/h
	mat->largeur = largeur;
	mat->hauteur = hauteur;
	
	return mat;
} 

void afficher_matrice(matrice *mat){

	int i, j;
	float ** contenu = NULL;
	
	//Précond.
	if(mat == NULL){
	
		fprintf(stderr, "[afficher_matrice] Le pointeur de matrice est à NULL, abandon");
		return;
	}
	
	//Init.
	contenu = mat->contenu;
	
	//Traitement
	printf("\n");
	
	for(i=0; i<(mat->hauteur) ; i++){
		
		for(j=0; j<(mat->largeur); j++){
		
			printf("%."NBDIGITS"f ", contenu[i][j]);
		}
		printf("\n");
	}
	
	printf("\n");
}

void nettoyer_matrice(matrice *mat){

	int i;
	float ** contenu = NULL;
	
	//Précond.
	if(mat == NULL){
	
		fprintf(stderr, "[nettoyer_matrice] Le pointeur de matrice est à NULL, abandon");
		return;
	}
	
	//Init.
	contenu = mat->contenu;
	
	//Traitement
	for(i=0; i<(mat->hauteur); i++){
	
		free(contenu[i]);
	}
	
	free(contenu);
	free(mat);
}

int affecter_matrice(matrice *mat, float nombre, int i, int j){

	//Précond
	if(mat == NULL || i < 0 || j < 0 || i>=(mat->hauteur) || j>=(mat->largeur) ){
	
		fprintf(stderr, "[affecter_matrice] Erreur dans les paramètres");
		return -1;
	}
	
	//Traitement
	mat->contenu[i][j] = nombre;
	return 0;
}

void saisie_matrice(matrice *mat){

	int hauteur, largeur, i, j;
	float val;
	
	//Précond
	if(mat == NULL){
	
		fprintf(stderr, "[saisie_matrice] Erreur dans les paramètres");
		return;
	}
	
	//Init.
	hauteur = mat-> hauteur;
	largeur = mat-> largeur;
	
	//Traitement
	for(i=0; i<hauteur; i++){
	
		for(j=0; j<largeur; j++){
		
			printf("Veuillez entrer la valeur de la case ligne %d colonne %d de la matrice : ", i, j);
			scanf("%f", &val);
			affecter_matrice(mat, val, i, j);
		}
	}
	
	printf("\nSaisie terminée\n");
}

void randomiser_matrice(matrice *mat, int min, int max){

	int hauteur, largeur, i, j;
	float val;
	
	//Précond.
	if(mat == NULL){
	
		fprintf(stderr, "[randomiser_matrice] Erreur dans les paramètres");
		return;
	}
	
	//Init.
	srand(time(NULL));
	hauteur = mat->hauteur;
	largeur = mat->largeur;
	
	//Traitement
	for(i=0; i<hauteur; i++){
	
		for(j=0; j<largeur; j++){

			val = rand()%(max-min)+min;
			affecter_matrice(mat, val, i, j);
		}
	}
}

float acceder_matrice(matrice *mat, int numLigne, int numColonne){

	// TODO Précond à ajouter
	return mat->contenu[numLigne][numColonne];
}

float ** ligne_matrice(matrice *mat, int numLigne){

	float ** ligne = NULL, *contenu = NULL;
	int j, largeur;
	
	//Précond.
	if(mat == NULL || numLigne < 0 || numLigne >= (mat->hauteur) ){
	
		fprintf(stderr, "[ligne_matrice] Erreur dans les paramètres");
		return NULL;
	}
	
	//Init.
	largeur = mat->largeur;
	contenu = mat->contenu[numLigne];
	
	//Traitement
	if((ligne = (float **)malloc(sizeof(float *)*largeur)) == NULL){
	
		perror("Erreur dans l'allocation d'une ligne");
		exit(ERREUR_MEM);
	}
	
	for(j=0; j<largeur; j++){
		
		ligne[j] = &(contenu[j]);
	}
	
	return ligne;
}

float ** colonne_matrice(matrice *mat, int numColonne){

	float ** colonne = NULL, **contenu = NULL;
	int i, hauteur;
	
	//Précond.
	if(mat == NULL || numColonne < 0 || numColonne >= (mat->largeur) ){
	
		fprintf(stderr, "[colonne_matrice] Erreur dans les paramètres");
		return NULL;
	}
	
	//Init.
	hauteur = mat->hauteur;
	contenu = mat->contenu;
	
	//Traitement
	if((colonne = (float **)malloc(sizeof(float *)*hauteur)) == NULL){
	
		perror("Erreur dans l'allocation d'une ligne");
		exit(ERREUR_MEM);
	}
	
	for(i=0; i<hauteur; i++){
		
		colonne[i] = &(contenu[i][numColonne]);
	}
	
	return colonne;
}

void afficher_colonne_matrice(matrice *mat, int numColonne){

	float ** colonne;
	int hauteur, i;
	
	//Précond.
	if((colonne = colonne_matrice(mat, numColonne)) == NULL){
	
		fprintf(stderr, "[afficher_colonne_matrice] Erreur dans la récupération de la colonne\n");
		return;
	}
	
	//Init.
	hauteur = mat->hauteur;
	
	//Traitement
	printf("Colonne %d : ", numColonne);
	
	for(i=0; i<hauteur; i++){
	
		printf("%."NBDIGITS"f ", *(colonne[i]));
	}
	
	printf("\n");
	
	free(colonne);
}

void afficher_ligne_matrice(matrice *mat, int numLigne){

	float ** ligne;
	int largeur, j;
	
	//Précond.
	if((ligne = ligne_matrice(mat, numLigne)) == NULL){
	
		fprintf(stderr, "[afficher_ligne_matrice] Erreur dans la récupération de la ligne\n");
		return;
	}
	
	//Init.
	largeur = mat->largeur;
	
	//Traitement
	printf("Ligne %d : ", numLigne);
	
	for(j=0; j<largeur; j++){
	
		printf("%."NBDIGITS"f ", *(ligne[j]));
	}
	
	printf("\n");
	
	free(ligne);
}
