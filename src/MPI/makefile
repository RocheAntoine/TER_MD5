SHELL = /bin/sh
CC = mpicc -fopenmp
EXEC = main
SRC = $(EXEC).c
OPTIONS = -lm -lcrypto

OBJETS	= bf.o princ.o utils.o



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

utils.o: utils.c utils.h
	@echo "	Compilation de utils.c"
	@$(CC) -c utils.c $(OPTIONS)


