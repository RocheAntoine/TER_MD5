SHELL = /bin/sh
CC = mpicc
EXEC = mainMPI
SRC = $(EXEC).c
OPTIONS = -lm -lcrypto

OBJETS	= bf.o princ.o



all: $(EXEC)

$(EXEC): $(OBJETS)
	$(CC)  $(OBJETS) $(OPTIONS) -o $(EXEC)

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


