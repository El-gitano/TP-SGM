#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "commun.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(){

	int i, idMessagerie;
	
	message_t message;
	message.typeMessage = 1;
	
	if( (idMessagerie = msgget(NUM_FILE, IPC_CREAT | IPC_EXCL | 0660)) == -1){
	
		perror("Erreur lors de la création de la file de messages");
		exit(1);
	}
	
	printf("Démmarage de la réception des messages\n");
	
	for(i=0; i<NB_ITER; i++){

		if(msgrcv(idMessagerie, &message, sizeof(int), 0, 0) == -1){
		
			perror("Impossible de recevoir un message");
			exit(1);
		}
		
		printf("Réception de %i\n", message.valeur);
	}
	
	printf("Fin de la réception des messages, suppression de la file de messages\n");
	
	// Nettoyage
	if( msgctl(idMessagerie, IPC_RMID, 0) == -1){
	
		perror("Erreur lors de la suppression de la file de messages");
		exit(1);
	}
	
	return EXIT_SUCCESS;
}
