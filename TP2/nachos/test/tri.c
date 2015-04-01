#include "userlib/syscall.h"
#include "userlib/libnachos.h"

#define TAILLE_TAB 10

void afficher_tab(int *tab);

void afficher_tab(int *tab){

	int i;
	
	for(i=0; i<TAILLE_TAB; i++){
	
		n_printf("%i | ", tab[i]);
	}
	
	n_printf("\n");
}

int main (){
	
	int *tab;
	OpenFileId f;
	
	if( (f = Open("fileVal")) == -1){
	
		n_printf("Erreur dans l'ouverture du fichier\n");
		Exit(1);
	}
	
	if( (tab = (int *)Mmap(f, TAILLE_TAB*sizeof(int))) == -1){
	
		n_printf("Erreur dans la mapping du fichier\n");
		Exit(1);
	}
	
	n_printf("Avant tri (adresse de tab : %x et openfile %x)\n", tab, f);
	afficher_tab(tab);
	
	int i, p, j;
	int x;
	
	for(i =0 ; i<TAILLE_TAB; i++){
		
		x = tab[i];
		p = i-1;
		
		while(tab[p] > x && p-- > 0);
		
		p++;
		
		for(j = i-1; j>=p; j--){
		
			tab[j+1] = tab[j];
		}
		
		tab[p] = x;
	}
	
	n_printf("Après tri\n");
	afficher_tab(tab);
	
	//Close(f);
	Exit(0);
	return 0;
}
