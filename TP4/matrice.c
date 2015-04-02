#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "commun.h"

#define STEPS 10

int nbPageAccedes = 0;

// Handler accès page
void handler(int signum, siginfo_t *info, void *ptr){

	nbPageAccedes++;
	
	if(mprotect(info->si_addr - ((int)info->si_addr%getpagesize()), getpagesize(), PROT_READ) == -1){
	
		perror("Erreur dans l'appel de mprotect");
		exit(1);
	}
}

int main(){

	int i, index, f;
	float* retourMap;
	struct sigaction detournement, ancien;

	srand(time(NULL));
	
	// Ouverture du fichier
	if( (f=open("matriceFichier", O_RDONLY)) == -1){
	
		perror("Erreur dans l'ouverture du fichier contenant la matrice");
		exit(1);
	}
	
  	// Rediriger le signal
	detournement.sa_sigaction = handler;
	sigemptyset(&detournement.sa_mask);
	detournement.sa_flags = SA_SIGINFO;

	if(sigaction(SIGSEGV, &detournement, &ancien) == -1){
	
		perror("Erreur lors de la mise en place de la routine");
		exit(1);
	}

	// Mapper fichier
	if( (retourMap = (float *)mmap(0, getpagesize()*NBPAGES, PROT_NONE, MAP_SHARED, f, 0)) == -1){
	
		perror("Erreur dans le mapping du fichier");
		exit(1);
	}
	
	//Accès aléatoire
	for(i=0; i<STEPS; i++){
		
		index = rand()%( (getpagesize()*NBPAGES)/sizeof(float)); // Spécifique à notre fichier matrice
		printf("Matrice[%d] = %f\n", index, retourMap[index]);
	}
	
	printf("%f%% des pages ont été accédées (%d pages)\n", (nbPageAccedes/(float)NBPAGES)*100, nbPageAccedes);
	 
	//Unmapper fichier
	if(munmap(retourMap, getpagesize()*NBPAGES) == -1){
	
		perror("Erreur lors de l'unmapping du fichier");
		exit(1);
	}
	
	close(f);
	return EXIT_SUCCESS;
}
