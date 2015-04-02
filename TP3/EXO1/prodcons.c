#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "liste.h"

#define N 5
#define NB_ITER 3000

/* Prototypes */
int opBuf(struct sembuf *buf, int numSem, char action);
t_element *nextElem(t_element *current);

/* Retourne l'élément suivant de la liste chaînée */
t_element *nextElem(t_element *current){

	if(current == NULL){
	
		return NULL;
	}
	
	return current->suivant;
}

/* Permet de réaliser une opération sur un buffer de sémaphore */
int opBuf(struct sembuf *buf, int numSem, char action){

	// Erreur dans les paramètres
	if(buf == NULL || numSem <0 || (action != 'P' && action != 'V')){
	
		return -1;
	}
	
	buf->sem_num = numSem;
	buf->sem_flg = 0;
	
	switch(action){
	
		case 'V':
		
			buf->sem_op = 1;
			break;
			
		case 'P':
		
			buf->sem_op = -1;
			break;
	}
	
	return 0;
}

int main(){

	// Gestion synchro & mem. partagée
	int semId, memId, retourFork;
	int semLibre = 0, semPlein = 1;
	struct sembuf operationSem;
	
	int i;
	t_element *liste, *courant;
	
	/* Création et init. des sémaphores
		Position semLibre = 0
		Position semPlein = 1
	*/
	if( (semId = semget(IPC_PRIVATE, 2, 0660)) == -1){
	
		perror("Erreur lors de la création des sémaphores");
		exit(1);
	}

	if(semctl(semId, semLibre, SETVAL, N) == -1){
	
		perror("Erreur lors de l'initialisation de semLibre");
		exit(1);
	}

	if(semctl(semId, semPlein, SETVAL, 0) == -1){
	
		perror("Erreur lors de l'initialisation de semPlein");
		exit(1);
	}
	
	// Création du segment de mémoire partagé
	if( (memId = shmget(IPC_PRIVATE, N*sizeof(t_element), 0660)) == -1){
	
		perror("Erreur lors de la création du segment de mémoire partagé");
		exit(1);
	}
	
	if( (retourFork = fork()) == -1){
	
		perror("Erreur dans le fork");
		exit(1);
	}
	
	// Producteur
	else if(retourFork == 0){
	
		// Attachement
		if( (liste = (t_element *)shmat(memId, 0, SHM_RND)) == -1){
		
			perror("PRODUCTEUR : Impossible d'attacher la mémoire au processus");
			exit(1);
		}
		
		// Initialisation de la liste chaînée
		for(i=0; i<N; i++){
		
			liste[i].valeur = 0;
			liste[i].suivant = &liste[(i+1)%N];
		}
	
		courant = liste;
		
		for(i=0; i<NB_ITER; i++){
		
			// Acquérir semLibre
			if(opBuf(&operationSem, semLibre, 'P') == -1){
			
				fprintf(stderr, "PRODUCTEUR : Erreur dans le passage de paramètre à opBuf\n");
				exit(1);
			}
			
			if(semop(semId, &operationSem, 1) == -1){
			
				perror("PRODUCTEUR : Erreur dans l'acquisition de semLibre");
				exit(1);
			}
			
			// Ecrire de la valeur
			printf("Écriture de liste[%d] = %i\n", i%N, i);
			courant->valeur = i;
			
			if( (courant = nextElem(courant)) == NULL){
			
				fprintf(stderr, "PRODUCTEUR : Erreur lors du changement d'élément\n");
				exit(1);
			}
			
			// Libération semPlein
			if(opBuf(&operationSem, semPlein, 'V') == -1){
			
				fprintf(stderr, "PRODUCTEUR : Erreur dans le passage de paramètre à opBuf\n");
				exit(1);
			}
			
			if(semop(semId, &operationSem, 1) == -1){
			
				perror("PRODUCTEUR : Erreur dans la liberation de semPlein");
				exit(1);
			}
		}
	
		// Détachement
		if(shmdt(liste) == -1){
		
			perror("PRODUCTEUR : Impossible de détacher le segment mémoire");
			exit(1);
		}
		
		return EXIT_SUCCESS;
	}

	// Attachement
	if( (liste = (t_element *)shmat(memId, 0, SHM_RND)) == -1){
		
		perror("CONSOMMATEUR : Impossible d'attacher la mémoire au processus");
		exit(1);
	}
	
	courant = liste;
		
	// Consommateur
	for(i=0; i<NB_ITER; i++){
		
		// Acquérir semPlein
		if(opBuf(&operationSem, semPlein, 'P') == -1){
		
			fprintf(stderr, "CONSOMMATEUR : Erreur dans le passage de paramètre à opBuf\n");
			exit(1);
		}
		
		if(semop(semId, &operationSem, 1) == -1){
			
			perror("CONSOMMATEUR : Erreur dans l'acquisition de semPlein");
			exit(1);
		}
		
		// Lecture de la valeur
		printf("liste[%i] = %i\n", i%N, courant->valeur);
		
		if( (courant = nextElem(courant)) == NULL){
		
			fprintf(stderr, "CONSOMMATEUR : Erreur lors du changement d'élément\n");
			exit(1);
		}
			
		// Libération semLibre
		if(opBuf(&operationSem, semLibre, 'V') == -1){
		
			fprintf(stderr, "CONSOMMATEUR : Erreur dans le passage de paramètre à opBuf\n");
			exit(1);
		}
		
		if(semop(semId, &operationSem, 1) == -1){
			
			perror("CONSOMMATEUR : Erreur dans liberation de semLibre");
			exit(1);
		}
	}
	
	// Détachement
	if(shmdt(liste) == -1){
		
		perror("CONSOMMATEUR : Impossible de détacher le segment mémoire");
		exit(1);
	}
		
	// Nettoyage
	if(semctl(semId, 0, IPC_RMID, 0) == -1){
	
		perror("Erreur lors de la suppresion des sémaphores");
		exit(1);
	}
	
	if(shmctl(memId, 0, NULL) == -1){
	
		perror("Erreur dans la suppresion du segment mémoire partagé");
		exit(1);
	}
	
	return EXIT_SUCCESS;
}
