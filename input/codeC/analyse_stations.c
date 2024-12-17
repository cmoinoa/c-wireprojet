#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure pour un nœud AVL
typedef struct AVLNode {
    char station_id[20];
    int capacity;
    int consumption;
    int height;
    struct AVLNode *left, *right;
} AVLNode;

// Fonction pour créer un nouveau nœud
AVLNode* createNode(char* id, int capacity, int consumption) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    strcpy(node->station_id, id);
    node->capacity = capacity;
    node->consumption = consumption;
    node->height = 1;
    node->left = node->right = NULL;
    return node;
}

// Fonction pour calculer la hauteur d'un nœud
int height(AVLNode* node) {
    if (!node) return 0;
    return node->height;
}

// Fonction pour effectuer une rotation à droite
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T = x->right;
    x->right = y;
    y->left = T;
    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));
    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));
    return x;
}

// Fonction pour effectuer une rotation à gauche
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T = y->left;
    y->left = x;
    x->right = T;
    x->height = 1 + (height(x->left) > height(x->right) ? height(x->left) : height(x->right));
    y->height = 1 + (height(y->left) > height(y->right) ? height(y->left) : height(y->right));
    return y;
}

// Fonction pour calculer l'équilibre
double getBalance(AVLNode* node) {
    if (!node) return 0;
    return height(node->left) - height(node->right);
}

// Fonction pour insérer dans l'AVL
AVLNode* insert(AVLNode* node, char* id, int capacity, int consumption) {
    if (!node) return createNode(id, capacity, consumption);
    // Comparaison pour insertion
    if (strcmp(id, node->station_id) < 0)
        node->left = insert(node->left, id, capacity, consumption);
    else if (strcmp(id, node->station_id) > 0)
        node->right = insert(node->right, id, capacity, consumption);
    else {
        node->consumption += consumption;
        return node;
    }

    // Mise à jour de la hauteur
    node->height = 1 + (height(node->left) > height(node->right) ? height(node->left) : height(node->right));

    // Ré-équilibrage
    double balance = getBalance(node);

    // Cas d'équilibrage
    if (balance > 1 && strcmp(id, node->left->station_id) < 0)
        return rotateRight(node);
    if (balance < -1 && strcmp(id, node->right->station_id) > 0)
        return rotateLeft(node);
    if (balance > 1 && strcmp(id, node->left->station_id) > 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && strcmp(id, node->right->station_id) < 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

// Fonction pour écrire l'AVL dans un fichier
void writeInFile(AVLNode* node, FILE* file) {
    if (node) {
        writeInFile(node->left, file);
        fprintf(file, "%s:%d:%d\n", node->station_id, node->capacity, node->consumption);
        writeInFile(node->right, file);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }
    
    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    if (!input || !output) {
        fprintf(stderr, "Error opening files\n");
        return 2;
    }

    char line[256], id[20];
    int capacity, consumption;
    AVLNode* root = NULL;

    while (fgets(line, sizeof(line), input)) {
        sscanf(line, "%[^:]:%d:%d", id, &capacity, &consumption);
        root = insert(root, id, capacity, consumption);
    }

    writeInFile(root, output);
    fclose(input);
    fclose(output);
    return 0;
}
