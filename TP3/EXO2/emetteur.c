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
	
	if( (idMessagerie = msgget(NUM_FILE, 0660)) == -1){
	
		perror("Erreur lors de la création de la file de messages");
		exit(1);
	}
	
	printf("Début de l'émetteur\n");
	
	for(i=0; i<NB_ITER; i++){

		message.valeur = i;
		
		printf("Envoi %i\n", i);
		if(msgsnd(idMessagerie, &message, sizeof(int), 0) == -1){
		
			perror("Impossible d'envoyer le message i");
			exit(1);
		}
	}
	
	printf("Fin de l'envoi des messages\n");
	
	return EXIT_SUCCESS;
}
