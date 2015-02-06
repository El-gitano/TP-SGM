#include "userlib/syscall.h"
#include "userlib/libnachos.h"

#define TAILLE_TAB 257

void reception_chaine(){

	char s[TAILLE_TAB];
	int ind_reception;
	
	if((ind_reception = TtyReceive(s, TAILLE_TAB)) < 0){
	
		PError("ERREUR: Reception de la chaine de caratere");
		Exit(1);
	}
	
	n_printf("Chaine reçue (%d)\n", ind_reception, s);
}

void thread(){

	reception_chaine();
	Exit(0);
}

int main (){

	ThreadId th1, th2, th3;
	
	n_printf("Debut programme reception\n");
	
	//Init. des threads
	if((th1 = threadCreate("thread1", thread)) == -1){
	
		PError("Erreur lors de la création du thread 1");
		Exit(1);
	}
	
	if(Join(th1) == -1){
	
		PError("Erreur dans l'attente du thread 1");
		Exit(1);
	}
	
	if((th2 = threadCreate("thread2", thread)) == -1){
	
		PError("Erreur lors de la création du thread 2");
		Exit(1);
	}
	
	if(Join(th2) == -1){
	
		PError("Erreur dans l'attente du thread 2");
		Exit(1);
	}
	
	if((th3 = threadCreate("thread3", thread)) == -1){
	
		PError("Erreur lors de la création du thread 3");
		Exit(1);
	}
	
	if(Join(th3) == -1){
	
		PError("Erreur dans l'attente du thread 3");
		Exit(1);
	}
	
	n_printf("Fin programme reception\n");
	return 0;
}
