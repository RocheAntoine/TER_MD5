SHELL = /bin/sh
CC = mpicc -O3 -fopenmp
EXEC = mainICC
SRC = $(EXEC).c
OPTIONS = -lm -lcrypto

OBJETS	= bf.o princ.o



all: $(EXEC)

$(EXEC): $(OBJETS)
	$(CC)  $(OBJETS) -o $(EXEC) $(OPTIONS)

clean:
	rm -f *.o *~ .*~ > /dev/null

#########################################################################
#	Les cibles
#########################################################################

bf.o: bf.c bf.h 
	@echo "	Compilation de bf.c"
	@$(CC) -c bf.c $(OPTIONS)


princ.o: princ.c bf.h
	@echo "	Compilation de princ.c"
	@$(CC) -c princ.c $(OPTIONS)


