# TP8 : Fichier Makefile
#
include ../Makefile.inc

EXE = serveur user #prodcons

all: ${EXE}

${EXE): ${PSE_LIB}

prodcons: prodcons.o tampon.o

clean:
	rm -f *.o *~ ${EXE}
