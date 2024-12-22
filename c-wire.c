#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Station {
    int id;
    double capacite;
    double consommation_comp;
    double consommation_indiv;
    struct Station *suivant;
} Station;

// Fonction pour créer une nouvelle station
Station* creer_station(int id, double capacite, double consommation_comp, double consommation_indiv) {
    Station *nouvelle_station = (Station*)malloc(sizeof(Station));
    if (!nouvelle_station) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(1);
    }
    nouvelle_station->id = id;
    nouvelle_station->capacite = capacite;
    nouvelle_station->consommation_comp = consommation_comp;
    nouvelle_station->consommation_indiv = consommation_indiv;
    nouvelle_station->suivant = NULL;
    return nouvelle_station;
}

// Fonction pour insérer une station dans une liste triée par capacité
Station* inserer_station_triee(Station *tete, Station *nouvelle_station) {
    if (!tete || nouvelle_station->capacite < tete->capacite) {
        nouvelle_station->suivant = tete;
        return nouvelle_station;
    }
    Station *courant = tete;
    while (courant->suivant && courant->suivant->capacite < nouvelle_station->capacite) {
        courant = courant->suivant;
    }
    nouvelle_station->suivant = courant->suivant;
    courant->suivant = nouvelle_station;
    return tete;
}

// Fonction pour vérifier si une ligne correspond aux critères
int ligne_valide(char **tokens, const char *type_station, const char *type_consommateur) {
    if (strcmp(type_station, "hvb") == 0 && strcmp(type_consommateur, "comp") == 0) {
        return strcmp(tokens[1], "-") != 0 && strcmp(tokens[2], "-") == 0 &&
               strcmp(tokens[3], "-") == 0 && strcmp(tokens[4], "-") == 0 &&
               strcmp(tokens[5], "-") == 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "hva") == 0 && strcmp(type_consommateur, "comp") == 0) {
        return strcmp(tokens[1], "-") != 0 && strcmp(tokens[2], "-") != 0 &&
               strcmp(tokens[3], "-") == 0 && strcmp(tokens[4], "-") == 0 &&
               strcmp(tokens[5], "-") == 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "lv") == 0 && strcmp(type_consommateur, "comp") == 0) {
        return strcmp(tokens[1], "-") == 0 && strcmp(tokens[3], "-") != 0 &&
               strcmp(tokens[4], "-") != 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "lv") == 0 && strcmp(type_consommateur, "indiv") == 0) {
        return strcmp(tokens[1], "-") == 0 && strcmp(tokens[3], "-") != 0 &&
               strcmp(tokens[5], "-") != 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "lv") == 0 && strcmp(type_consommateur, "all") == 0) {
        return strcmp(tokens[0], "-") != 0 && strcmp(tokens[3], "-") != 0 &&
               strcmp(tokens[6], "-") != 0;
    }
    return 0;
}

// Fonction pour lire et traiter les données du fichier
void traiter_fichier(FILE *fichier, Station **tete, const char *type_station, const char *type_consommateur) {
    char line[256];
    while (fgets(line, sizeof(line), fichier)) {
        char *tokens[8];
        char *token = strtok(line, ";");
        int i = 0;

        // Découper la ligne en tokens
        while (token && i < 8) {
            tokens[i++] = token;
            token = strtok(NULL, ";");
        }

        // Vérifier que la ligne a le bon nombre de colonnes
        if (i < 8) continue;

        // Valider la ligne selon les critères
        if (ligne_valide(tokens, type_station, type_consommateur)) {
            int id = atoi(tokens[0]);
            double capacite = strcmp(tokens[6], "-") == 0 ? 0.0 : atof(tokens[6]);
            double comp = strcmp(tokens[4], "-") == 0 ? 0.0 : atof(tokens[4]);
            double indiv = strcmp(tokens[5], "-") == 0 ? 0.0 : atof(tokens[5]);

            Station *nouvelle_station = creer_station(id, capacite, comp, indiv);
            *tete = inserer_station_triee(*tete, nouvelle_station);
        }
    }
}

// Fonction pour écrire l'entête descriptive
void ecrire_entete(FILE *output, const char *type_station, const char *type_consommateur) {
    fprintf(output, "                                           \n");         //problème d'affichage sinon
    fprintf(output, "Station ");
    if (strcmp(type_station, "hvb") == 0) fprintf(output, "HVB");
    else if (strcmp(type_station, "hva") == 0) fprintf(output, "HVA");
    else if (strcmp(type_station, "lv") == 0) fprintf(output, "LV");

    fprintf(output, " / Capacité / ");
    if (strcmp(type_consommateur, "comp") == 0) fprintf(output, "Consommation (entreprises)\n");
    else if (strcmp(type_consommateur, "indiv") == 0) fprintf(output, "Consommation (particuliers)\n");
    else if (strcmp(type_consommateur, "all") == 0) fprintf(output, "Consommation (tous)\n");
}

// Fonction pour imprimer les résultats dans un fichier
void imprimer_resultats(Station *tete, FILE *output) {
    Station *courant = tete;
    while (courant) {
        if (courant->capacite > 0.0) {
            fprintf(output, "%d;%.0lf;%.0lf\n", courant->id, courant->capacite,
                    courant->consommation_comp + courant->consommation_indiv);
        }
        courant = courant->suivant;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage : %s <fichier_donnees> <type_station> <type_consommateur>\n", argv[0]);
        return 1;
    }

    char *fichier_donnees = argv[1];
    char *type_station = argv[2];
    char *type_consommateur = argv[3];

    FILE *fichier = fopen(fichier_donnees, "r");
    if (!fichier) {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    Station *tete = NULL;
    traiter_fichier(fichier, &tete, type_station, type_consommateur);
    fclose(fichier);

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "tmp/%s_%s.csv", type_station, type_consommateur);
    FILE *output = fopen(output_filename, "w");
    if (!output) {
        perror("Erreur d'ouverture du fichier de sortie");
        return 1;
    }

    ecrire_entete(output, type_station, type_consommateur);
    imprimer_resultats(tete, output);
    fclose(output);

    printf("Fichier généré : %s\n", output_filename);
    return 0;
}
