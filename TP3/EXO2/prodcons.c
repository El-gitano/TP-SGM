#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commun.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(){

	int idMessagerie;
	int i, retourFork;
	
	
	
	if( (retourFork = fork()) == -1){
	
		perror("Erreur dans le fork");
		exit(1);
	}
	// Producteur
	else if(retourFork == 0){
		
		
		
		return EXIT_SUCCESS;
	}

	// Consommateur
	for(i=0; i<NB_ITER; i++){
		
		// Lecture de la valeur
		printf("tab[%i] = %i\n", i%N, tab[i%N]);
	}
	
		
	// Nettoyage
	
	return EXIT_SUCCESS;
}
