LIBS= creerMatrice.c matrice-lib.c
OBJ= creerMatrice.o matrice-lib.o
HEADERS= matrice-lib.h
FLAGS= -Wall -g
EXE= creerMatrice

all: compile link

link: compile

	gcc ${OBJ} -o ${EXE} ${FLAGS}
	
compile: ${LIBS} ${HEADERS}

	gcc -c ${LIBS} ${FLAGS}
	
clean: ${OBJ} ${EXE}

	rm -f ${OBJ} ${EXE}
