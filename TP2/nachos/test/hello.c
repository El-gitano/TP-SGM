#include "userlib/syscall.h"
#include "userlib/libnachos.h"

int main(){

	int i=10;

	n_printf("Hello ");
	n_printf("world !\n");
	
	i = i+4;
	i = i+10;

	Exit(0);
	return 0;
}
