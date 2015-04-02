SOURCES= creerMatrice.c matrice-lib.c matrice.c
OBJ1= creerMatrice.o matrice-lib.o
OBJ2= matrice.o
FLAGS= -Wall -g

all: creerMatrice matrice

creerMatrice: ${OBJ1} compile

	gcc ${OBJ1} -o creerMatrice ${FLAGS}

matrice: ${OBJ2} compile

	gcc ${OBJ2} -o matrice ${FLAGS}
	
compile: ${SOURCES}

	gcc -c ${SOURCES} ${FLAGS}
	
clean: ${OBJ} ${EXE}

	rm -f ${OBJ1} ${OBJ2} creerMatrice matrice matriceFichier
