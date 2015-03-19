#include "userlib/syscall.h"
#include "userlib/libnachos.h"

SemId attente;

void threadAttente(){
	
	if(P(attente) == -1){

		PError("Erreur dans l'op√ration P sur la s√maphore d'attente");
		Exit(1);
	}

	n_printf("Deblocage du thread\n");
	Exit(0);
}

void threadDeblocage(){

	int i;

	for(i=0; i<2; i++){

		if(V(attente) == -1){

			PError("Erreur dans l'op√ration V sur la s√maphore d'attente");
			Exit(1);
		}
	}
	n_printf("Deblocage des threads pour le rendez-vous\n");
	Exit(0);
}

int main (){
	
	if((attente=SemCreate("sem",0)) == -1){

		PError("Erreur dans la cr√ation du s√maphore");
		Exit(1);
	}
	
	char* name = "thr";
	n_printf("Debut du test\n");
	ThreadId id1;
	ThreadId id2;
	ThreadId id3;

	if ( (id1 = threadCreate(name,threadAttente))==-1){
		n_printf("Erreur a la creation du threads 1\n");
		Exit(1);
	}
	if ( (id2 = threadCreate(name,threadAttente))==-1){
		n_printf("Erreur a la creation du threads 2\n");
		Exit(1);
	}

	n_printf("Pour effectuer le rendez-vous, appuyez sur une touche");
	n_read_int();

	if ( (id3 = threadCreate(name, threadDeblocage))==-1){
		n_printf("Erreur a la creation du threads 3\n");
		Exit(1);
	}
	if (Join(id1)==-1){
		n_printf("Erreur dans l'attente du threads 1\n");
		Exit(1);
	}
	if (Join(id2)==-1){
		n_printf("Erreur dans l'attente du threads 2\n");
		Exit(1);
	}
	if (Join(id3)==-1){
		n_printf("Erreur dans l'attente du threads 3\n");
		Exit(1);
	}
	n_printf("\nFin du test");
	
	return 0;
}


