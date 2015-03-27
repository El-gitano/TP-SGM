#include "userlib/syscall.h"
#include "userlib/libnachos.h"
#define TAILLE_TAB 4096

int main(){

	char tab1[TAILLE_TAB], tab2[TAILLE_TAB];

	n_printf("Début de la copie\n");

	n_memcpy(tab2, tab1, TAILLE_TAB);

	n_printf("Fin de la copie\n");

	Exit(0);
	return 0;
}
