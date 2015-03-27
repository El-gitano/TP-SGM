#include "userlib/syscall.h"
#include "userlib/libnachos.h"

void envoi_chaine(char *s){

	int ind_envoi = TtySend(s);
	
	//On compare le retour de la fonction à la taille de la chaîne
	if(ind_envoi != (n_strlen(s)+1) && ind_envoi != 256 ){
	
		n_printf("Erreur dans l'envoi (ind_envoi = %d)\n", s, ind_envoi);
		Exit(1);
	}
	else{
	
		n_printf("Envoi de %d caractères avec succès \n", ind_envoi, s);
	}
}

//Chaîne de 256 car. pour tester les limites
void thread2(){

	envoi_chaine("DHPXPAFNQQKPZVUDAMNKJLSARZFLVCPXEQXIZRXPCLNRXPALJEYMIOITLMUJIJUOEIIFMNEMJKNWWAJYCLBKCKNIVICFCMPFLGPNKCVUQFIFQIDYIMORQTHSBOSPVJVXMVEBIZNNWAMXOXYSBGMLRHBAZTDJOHCKASYQWRKXCNDMSAFBBPQENNNMMWVVXDFDSWSKFOFIXULHAFPLYHPCWBTOAMQNHQVFJYITGHKJHTVDWLTGJLSWSAMIUIKYTQMZ");
	Exit(0);
}

void thread3(){

	envoi_chaine("Chaine2");
	Exit(0);
}

void thread1(){

	envoi_chaine("C");
	Exit(0);
}

int main (){

	ThreadId th1, th2, th3;
	
	n_printf("Debut programme emission\n");
	
	//Init. des threads
	if((th1 = threadCreate("thread1", thread1)) == -1){
	
		PError("Erreur lors de la création du thread 1");
		Exit(1);
	}
	
	if(Join(th1) == -1){
	
		PError("Erreur dans l'attente du thread 1");
		Exit(1);
	}
	
	n_printf("Appuyez sur une touche pour envoyer la chaîne suivante : ");
	n_read_int();
	
	if((th2 = threadCreate("thread2", thread2)) == -1){
	
		PError("Erreur lors de la création du thread 2");
		Exit(1);
	}
	
	if(Join(th2) == -1){
	
		PError("Erreur dans l'attente du thread 2");
		Exit(1);
	}
	
	n_printf("Appuyez sur une touche pour envoyer la chaîne suivante : ");
	n_read_int();
	
	if((th3 = threadCreate("thread3", thread3)) == -1){
	
		PError("Erreur lors de la création du thread 3");
		Exit(1);
	}
	
	if(Join(th3) == -1){
	
		PError("Erreur dans l'attente du thread 3");
		Exit(1);
	}

	n_printf("Fin programme emission");
	return 0;
}
