CC = gcc
CFLAGS = -Wall -O2
SRC = c-wire.c
OBJ = c-wire.o
EXEC = c-wire

# Règle pour la compilation du programme C
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

$(OBJ): c-wire.c
	$(CC) $(CFLAGS) -c c-wire.c

# Règle pour nettoyer les fichiers générés
clean:
	rm -f $(EXEC) $(OBJ)
	rm -f tmp/*.csv

# Règle pour forcer la recompilation
rebuild: clean all
