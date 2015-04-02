#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "matrice-lib.h"
#include "commun.h"

#define LARG 32*NBPAGES
#define HAUT 32*NBPAGES

int main(){

	int i, j;
	float temp;
	FILE *f;
	
	matrice *mat = creer_matrice(HAUT, LARG);
	
	if( (f = fopen("matriceFichier", "w")) == NULL){
	
		perror("Erreur ouverture");
		exit(1);
	}
	
	for(i=0; i<HAUT ; i++){
		
		for(j=0; j<LARG; j++){
			
			temp = acceder_matrice(mat, i, j);
			if(fwrite(&temp , sizeof(float), 1, f) != 1){
			
				perror("Erreur écriture");
				exit(1);
			}
		}
	}
	
	nettoyer_matrice(mat);
	fclose(f);
	
	return EXIT_SUCCESS;
}
