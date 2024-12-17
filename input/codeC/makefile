CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = analyse_stations

all: $(TARGET)

$(TARGET): analyse_stations.o
	$(CC) $(CFLAGS) -o $(TARGET) analyse_stations.o

analyse_stations.o: analyse_stations.c
