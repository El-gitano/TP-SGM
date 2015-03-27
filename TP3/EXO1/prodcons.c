#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define N 5
#define NB_ITER 3000

struct sembuf prepaBuf(int numSem, char action);

struct sembuf prepaBuf(int numSem, char action){

	struct sembuf res;
	 
	res.sem_num = numSem;
	res.sem_flg = 0;
	
	switch(action){
	
		case 'V':
		
			res.sem_op = 1;
			break;
			
		case 'P':
		
			res.sem_op = -1;
			break;
	}
	
	return res;
}

int main(){

	// Gestion synchro & mem. partagée
	int semId, memId, retourFork;
	int semLibre = 0, semPlein = 1;
	struct sembuf operationSem;
	
	int i, *tab;
	
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
	if( (memId = shmget(IPC_PRIVATE, N*sizeof(int), 0660)) == -1){
	
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
		if( (tab = (int *)shmat(memId, 0, SHM_RND)) == -1){
		
			perror("PRODUCTEUR : Impossible d'attacher la mémoire au processus");
			exit(1);
		}
		
		for(i=0; i<NB_ITER; i++){
		
			// Acquérir semLibre
			operationSem = prepaBuf(semLibre, 'P');
			if(semop(semId, &operationSem, 1) == -1){
			
				perror("PRODUCTEUR : Erreur dans l'acquisition de semLibre");
				exit(1);
			}
			
			// Ecrire de la valeur
			tab[i%N] = i;
			
			// Libération semPlein
			operationSem = prepaBuf(semPlein, 'V');
			if(semop(semId, &operationSem, 1) == -1){
			
				perror("PRODUCTEUR : Erreur dans la liberation de semPlein");
				exit(1);
			}
		}
	
		// Détachement
		if(shmdt(tab) == -1){
		
			perror("PRODUCTEUR : Impossible de détacher le segment mémoire");
			exit(1);
		}
		
		return EXIT_SUCCESS;
	}

	// Attachement
	if( (tab = (int *)shmat(memId, 0, SHM_RND)) == -1){
		
		perror("CONSOMMATEUR : Impossible d'attacher la mémoire au processus");
		exit(1);
	}
		
	// Consommateur
	for(i=0; i<NB_ITER; i++){
		
		// Acquérir semPlein
		operationSem = prepaBuf(semPlein, 'P');
		if(semop(semId, &operationSem, 1) == -1){
			
			perror("CONSOMMATEUR : Erreur dans l'acquisition de semPlein");
			exit(1);
		}
		
		// Lecture de la valeur
		printf("tab[%i] = %i\n", i%N, tab[i%N]);
		
		// Libération semLibre
		operationSem = prepaBuf(semLibre, 'V');
		if(semop(semId, &operationSem, 1) == -1){
			
			perror("CONSOMMATEUR : Erreur dans liberation de semLibre");
			exit(1);
		}
	}
	
	// Détachement
	if(shmdt(tab) == -1){
		
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
