#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ID_LENGTH 50
#define MAX_TYPE_LENGTH 10

// Structure pour une station (nœud de l'AVL)
typedef struct Station {
    char id[MAX_ID_LENGTH];
    char type_station[MAX_TYPE_LENGTH];
    char type_conso[MAX_TYPE_LENGTH];
    double consommation;
    struct Station *left, *right;
    int height;
} Station;

// Fonction pour obtenir la hauteur d'un nœud
int hauteur(Station *node) {
    return node ? node->height : 0;
}

// Fonction pour obtenir le facteur de balance d'un nœud
int facteur_balance(Station *node) {
    return hauteur(node->left) - hauteur(node->right);
}

// Fonction pour effectuer une rotation à droite
Station* rotation_droite(Station *y) {
    Station *x = y->left;
    Station *T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = 1 + (hauteur(y->left) > hauteur(y->right) ? hauteur(y->left) : hauteur(y->right));
    x->height = 1 + (hauteur(x->left) > hauteur(x->right) ? hauteur(x->left) : hauteur(x->right));
    
    return x;
}

// Fonction pour effectuer une rotation à gauche
Station* rotation_gauche(Station *x) {
    Station *y = x->right;
    Station *T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = 1 + (hauteur(x->left) > hauteur(x->right) ? hauteur(x->left) : hauteur(x->right));
    y->height = 1 + (hauteur(y->left) > hauteur(y->right) ? hauteur(y->left) : hauteur(y->right));
    
    return y;
}

// Fonction pour rééquilibrer l'arbre AVL
Station* equilibrer(Station *node) {
    int balance = facteur_balance(node);

    // Rotation à droite
    if (balance > 1 && strcmp(node->id, node->left->id) > 0)
        return rotation_droite(node);

    // Rotation à gauche
    if (balance < -1 && strcmp(node->id, node->right->id) < 0)
        return rotation_gauche(node);

    // Rotation gauche-droite
    if (balance > 1 && strcmp(node->id, node->left->id) < 0) {
        node->left = rotation_gauche(node->left);
        return rotation_droite(node);
    }

    // Rotation droite-gauche
    if (balance < -1 && strcmp(node->id, node->right->id) > 0) {
        node->right = rotation_droite(node->right);
        return rotation_gauche(node);
    }

    return node;
}

// Fonction d'insertion dans l'arbre AVL
Station* inserer(Station *node, const char *id, const char *type_station, const char *type_conso, double consommation) {
    if (node == NULL) {
        // Créer un nouveau nœud pour la station
        Station *newNode = (Station*)malloc(sizeof(Station));
        if (newNode == NULL) {
            perror("Erreur d'allocation mémoire");
            exit(EXIT_FAILURE);
        }
        strncpy(newNode->id, id, MAX_ID_LENGTH - 1);
        strncpy(newNode->type_station, type_station, MAX_TYPE_LENGTH - 1);
        strncpy(newNode->type_conso, type_conso, MAX_TYPE_LENGTH - 1);
        newNode->consommation = consommation;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;  // Hauteur initiale
        return newNode;
    }

    if (strcmp(id, node->id) == 0) {
        // Si l'identifiant existe déjà, mettre à jour la consommation
        node->consommation += consommation;
    } else if (strcmp(id, node->id) < 0) {
        node->left = inserer(node->left, id, type_station, type_conso, consommation);
    } else {
        node->right = inserer(node->right, id, type_station, type_conso, consommation);
    }

    // Mise à jour de la hauteur du nœud actuel
    node->height = 1 + (hauteur(node->left) > hauteur(node->right) ? hauteur(node->left) : hauteur(node->right));
    
    // Rééquilibrer l'arbre AVL
    return equilibrer(node);
}

// Fonction pour parcourir l'arbre et afficher les résultats
void parcourir_et_afficher(Station *node, FILE *fichier) {
    if (node != NULL) {
        parcourir_et_afficher(node->left, fichier);
        fprintf(fichier, "%s,%s,%s,%.2f\n", node->id, node->type_station, node->type_conso, node->consommation);
        parcourir_et_afficher(node->right, fichier);
    }
}

// Fonction principale
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <chemin_fichier_input> <chemin_fichier_output>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *chemin_fichier_input = argv[1];
    const char *chemin_fichier_output = argv[2];

    // Ouvrir le fichier d'entrée
    FILE *fichier_input = fopen(chemin_fichier_input, "r");
    if (!fichier_input) {
        perror("Erreur d'ouverture du fichier d'entrée");
        return EXIT_FAILURE;
    }

    // Création de l'arbre AVL
    Station *root = NULL;
    char id[MAX_ID_LENGTH], type_station[MAX_TYPE_LENGTH], type_conso[MAX_TYPE_LENGTH];
    double consommation;

    // Lire les données et les insérer dans l'arbre
    while (fscanf(fichier_input, "%s %s %s %lf", id, type_station, type_conso, &consommation) == 4) {
        root = inserer(root, id, type_station, type_conso, consommation);
    }

    fclose(fichier_input);

    // Ouvrir le fichier de sortie
    FILE *fichier_output = fopen(chemin_fichier_output, "w");
    if (!fichier_output) {
        perror("Erreur d'ouverture du fichier de sortie");
        return EXIT_FAILURE;
    }

    // Parcourir l'arbre et écrire les résultats dans le fichier de sortie
    parcourir_et_afficher(root, fichier_output);

    fclose(fichier_output);

    // Libérer la mémoire allouée à l'arbre AVL
    free(root);

    return EXIT_SUCCESS;
}

