# TP8 : Fichier Makefile
#
include ../Makefile.inc

EXE = serveur client #prodcons 

all: ${EXE}

${EXE): ${PSE_LIB}

prodcons: prodcons.o tampon.o

clean:
	rm -f *.o *~ ${EXE}
