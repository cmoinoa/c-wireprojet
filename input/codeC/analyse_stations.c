#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Définir la taille maximale des chaînes de caractères
#define MAX_LINE_LENGTH 1024

// Structure pour stocker une station d'énergie
typedef struct {
    char id[50];
    char type_station[10];
    char type_conso[10];
    // D'autres champs peuvent être ajoutés selon les besoins
} Station;

// Fonction pour lire les données depuis un fichier et les stocker dans un tableau dynamique
Station* lire_donnees(const char *chemin_fichier, int *nombre_stations) {
    FILE *fichier = fopen(chemin_fichier, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Calculer le nombre de stations (ici simplifié à la lecture)
    int capacity = 10; // Capacité initiale
    *nombre_stations = 0;
    Station *stations = malloc(capacity * sizeof(Station));
    if (!stations) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
    }

    char ligne[MAX_LINE_LENGTH];
    while (fgets(ligne, sizeof(ligne), fichier)) {
        // Lecture des données de chaque ligne (ici, on suppose un format fixe)
        if (*nombre_stations >= capacity) {
            capacity *= 2; // Double la capacité pour éviter un dépassement
            stations = realloc(stations, capacity * sizeof(Station));
            if (!stations) {
                perror("Erreur de réallocation mémoire");
                exit(EXIT_FAILURE);
            }
        }

        // Par exemple, ici nous analysons chaque ligne pour extraire les champs
        sscanf(ligne, "%49s %9s %9s", stations[*nombre_stations].id, stations[*nombre_stations].type_station, stations[*nombre_stations].type_conso);
        (*nombre_stations)++;
    }

    fclose(fichier);
    return stations;
}

// Fonction pour écrire les résultats dans un fichier CSV
void ecrire_resultats(const char *chemin_fichier, Station *stations, int nombre_stations) {
    FILE *fichier = fopen(chemin_fichier, "w");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier de sortie");
        exit(EXIT_FAILURE);
    }

    // Écrire les entêtes du CSV
    fprintf(fichier, "ID,Type Station,Type Conso\n");

    // Écrire les données
    for (int i = 0; i < nombre_stations; i++) {
        fprintf(fichier, "%s,%s,%s\n", stations[i].id, stations[i].type_station, stations[i].type_conso);
    }

    fclose(fichier);
}

// Fonction principale
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <chemin_fichier_input> <chemin_fichier_output>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *chemin_fichier_input = argv[1];
    const char *chemin_fichier_output = argv[2];

    // Lire les données depuis le fichier d'entrée
    int nombre_stations = 0;
    Station *stations = lire_donnees(chemin_fichier_input, &nombre_stations);

    // Écrire les résultats dans un fichier CSV
    ecrire_resultats(chemin_fichier_output, stations, nombre_stations);

    // Libérer la mémoire allouée
    free(stations);

    return EXIT_SUCCESS;
}
