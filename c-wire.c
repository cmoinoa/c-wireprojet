#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Station {
    int id;
    double capacite;
    double consommation_comp;
    double consommation_indiv;
    struct Station *gauche;
    struct Station *droite;
} Station;

// Fonction pour créer un nouveau noeud de l'arbre
Station* creer_station(int id, double capacite, double consommation_comp, double consommation_indiv) {
    Station *nouveau = (Station*)malloc(sizeof(Station));
    if (!nouveau) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(1);
    }
    nouveau->id = id;
    nouveau->capacite = capacite;
    nouveau->consommation_comp = consommation_comp;
    nouveau->consommation_indiv = consommation_indiv;
    nouveau->gauche = NULL;
    nouveau->droite = NULL;
    return nouveau;
}

// Fonction pour insérer une station dans l'arbre AVL
Station* inserer_station(Station *racine, Station *station) {
    if (racine == NULL) {
        return station;
    }
    if (station->id < racine->id) {
        racine->gauche = inserer_station(racine->gauche, station);
    } else {
        racine->droite = inserer_station(racine->droite, station);
    }
    return racine;
}

// Fonction pour vérifier si une ligne correspond aux critères
int ligne_valide(char **tokens, const char *type_station, const char *type_consommateur) {
    if (strcmp(type_station, "hvb") == 0 && strcmp(type_consommateur, "comp") == 0) {
        // hvb comp: valeur;valeur;vide;vide;vide;vide;valeur;vide
        return strcmp(tokens[1], "-") != 0 && strcmp(tokens[2], "-") == 0 &&
               strcmp(tokens[3], "-") == 0 && strcmp(tokens[4], "-") == 0 &&
               strcmp(tokens[5], "-") == 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "hva") == 0 && strcmp(type_consommateur, "comp") == 0) {
        // hva comp: valeur;valeur;valeur;vide;vide;vide;valeur;vide
        return strcmp(tokens[1], "-") != 0 && strcmp(tokens[2], "-") != 0 &&
               strcmp(tokens[3], "-") == 0 && strcmp(tokens[4], "-") == 0 &&
               strcmp(tokens[5], "-") == 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "lv") == 0 && strcmp(type_consommateur, "comp") == 0) {
        // lv comp: valeur;vide;vide;valeur;valeur;vide;valeur;vide
        return strcmp(tokens[1], "-") == 0 && strcmp(tokens[3], "-") != 0 &&
               strcmp(tokens[4], "-") != 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "lv") == 0 && strcmp(type_consommateur, "indiv") == 0) {
        // lv indiv: valeur;vide;vide;valeur;vide;valeur;valeur;vide
        return strcmp(tokens[1], "-") == 0 && strcmp(tokens[3], "-") != 0 &&
               strcmp(tokens[5], "-") != 0 && strcmp(tokens[6], "-") != 0;
    }
    if (strcmp(type_station, "lv") == 0 && strcmp(type_consommateur, "all") == 0) {
        // lv all: valeur;vide;vide;valeur;vide;vide;valeur;vide
        return strcmp(tokens[0], "-") != 0 && strcmp(tokens[3], "-") != 0 &&
               strcmp(tokens[6], "-") != 0;
    }
    return 0;
}

// Fonction pour lire et traiter les données du fichier
void traiter_fichier(FILE *fichier, Station **racine, const char *type_station, const char *type_consommateur) {
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
            *racine = inserer_station(*racine, nouvelle_station);
        }
    }
}

// Fonction pour imprimer les résultats dans un fichier
void imprimer_resultats(Station *racine, FILE *output) {
    if (racine == NULL) return;
    imprimer_resultats(racine->gauche, output);
    if (racine->capacite > 0.0) {
        fprintf(output, "%d:%.0lf:%.0lf\n", racine->id, racine->capacite, racine->consommation_comp + racine->consommation_indiv);
    }
    imprimer_resultats(racine->droite, output);
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

    Station *racine = NULL;
    traiter_fichier(fichier, &racine, type_station, type_consommateur);
    fclose(fichier);

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "tmp/%s_%s.csv", type_station, type_consommateur);
    FILE *output = fopen(output_filename, "w");
    if (!output) {
        perror("Erreur d'ouverture du fichier de sortie");
        return 1;
    }

    fprintf(output, "Station %s:Capacité:Consommation (%s)\n", type_station, type_consommateur);
    imprimer_resultats(racine, output);
    fclose(output);

    return 0;
}
