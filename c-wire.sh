#!/bin/bash

# Afficher l'aide du script
afficher_aide() {
    echo "Usage: ./c-wire.sh [chemin_fichier_dat] [type_station] [type_conso] [id_centrale (optionnel)] [-h]"
    echo "\nOptions :"
    echo "  chemin_fichier_dat   Chemin du fichier d'entrée (.dat) contenant les données"
    echo "  type_station         Type de station à analyser (hvb, hva, lv)"
    echo "  type_conso           Type de consommateur (comp, indiv, all)"
    echo "  id_centrale          (Optionnel) Filtre par identifiant de centrale"
    echo "  -h                   Affiche cette aide et ignore les autres paramètres"
    echo "\nExemples :"
    echo "  ./c-wire.sh input/c-wire_v00.dat hvb comp"
    echo "  ./c-wire.sh input/c-wire_v00.dat lv all 2"
    exit 0
}

# Fonction pour afficher un message d'erreur
erreur() {
    echo "Erreur: $1"
    exit 1
}

# Vérification des paramètres d'entrée
if [[ "$1" == "-h" ]]; then
    afficher_aide
fi

if [ $# -lt 3 ]; then
    erreur "Nombre de paramètres insuffisant."
fi

CHEMIN_FICHIER="$1"
TYPE_STATION="$2"
TYPE_CONSO="$3"
ID_CENTRALE="$4"

# Validation du type de station et des consommateurs
if [[ "$TYPE_STATION" != "hvb" && "$TYPE_STATION" != "hva" && "$TYPE_STATION" != "lv" ]]; then
    erreur "Type de station invalide."
fi
if [[ "$TYPE_CONSO" != "comp" && "$TYPE_CONSO" != "indiv" && "$TYPE_CONSO" != "all" ]]; then
    erreur "Type de consommateur invalide."
fi

# Vérification des dossiers
mkdir -p tmp tests
if [ -d tmp ]; then
    rm -rf tmp/*
else
    erreur "Le répertoire tmp n'existe pas ou n'a pas pu être créé."
fi

# Vérification de l'exécutable C
EXECUTABLE="input/codeC/analyse_stations"
if [ ! -f "$EXECUTABLE" ]; then
    erreur "L'exécutable C n'a pas été trouvé. Assurez-vous qu'il a été compilé."
fi

# Fonction de génération CSV
generer_csv() {
    local type_station="$1"
    local type_conso="$2"
    local fichier_tmp="tmp/${type_station}_${type_conso}.tmp"
    local resultat="tests/${type_station}_${type_conso}.csv"

    echo "Filtrage des données pour $type_station avec $type_conso..."
    awk -F":" -v type_station="$type_station" -v type_conso="$type_conso" \
    'NR == 1 || ($2 == type_station && $3 == type_conso)' "$CHEMIN_FICHIER" > "$fichier_tmp"
    
    if [ ! -s "$fichier_tmp" ]; then
        erreur "Le fichier temporaire $fichier_tmp est vide ou n'a pas pu être créé."
    fi

    echo "Exécution du programme C pour $type_station avec $type_conso..."
    $EXECUTABLE "$fichier_tmp" "$resultat"
    
    if [ $? -ne 0 ]; then
        erreur "Erreur lors de l'exécution du programme C."
    fi

    echo "Fichier généré : $resultat"
}

# Appels pour générer les fichiers CSV pour chaque combinaison
generer_csv "hvb" "comp"
generer_csv "hva" "comp"
generer_csv "lv" "comp"
generer_csv "lv" "indiv"
generer_csv "lv" "all"
generer_csv "lv" "all_minmax"

echo "Tous les fichiers ont été générés."
