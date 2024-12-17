#!/bin/bash

# Script c-wire.sh : analyse des stations d'energie
# Auteur : [Votre Nom]
# Date : [Date]
# Description : Ce script shell traite un fichier CSV volumineux, compile et exécute un programme C pour
#               analyser des stations HVB, HVA, et LV et générer des fichiers de sortie structurés.

# ---------------------- FONCTIONS --------------------------
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

# ------------------- VERIFICATION DES PARAMETRES ------------------

# Paramètre d'aide prioritaire
if [[ "$1" == "-h" ]]; then
    afficher_aide
fi

# Validation des paramètres obligatoires
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
if { [[ "$TYPE_STATION" == "hvb" || "$TYPE_STATION" == "hva" ]] && [[ "$TYPE_CONSO" != "comp" ]]; }; then
    erreur "Seuls les consommateurs 'comp' sont autorisés pour HVB et HVA."
fi

# -------------------- PREPARATION ENVIRONNEMENT ------------------

# Vérification des dossiers et nettoyage
mkdir -p tmp graphs tests input/codeC
if [ -d tmp ]; then
    rm -rf tmp/*
fi

# Vérification du fichier d'entrée
if [ ! -f "$CHEMIN_FICHIER" ]; then
    erreur "Le fichier d'entrée spécifié n'existe pas."
fi

# Vérification de l'exécutable C
EXECUTABLE="input/codeC/analyse_stations"
if [ ! -f "$EXECUTABLE" ]; then
    echo "Compilation de l'exécutable C..."
    (cd input/codeC && make)
    if [ $? -ne 0 ]; then
        erreur "Echec de la compilation du programme C."
    fi
fi

# ---------------------- TRAITEMENT PRINCIPAL ----------------------

# Filtrage des données (préparation temporaire)
FICHIER_TMP="tmp/${TYPE_STATION}_${TYPE_CONSO}.tmp"
echo "Filtrage des données pour $TYPE_STATION avec $TYPE_CONSO..."
awk -F":" 'NR == 1 || ($2 == "'$TYPE_STATION'" && $3 == "'$TYPE_CONSO'")' "$CHEMIN_FICHIER" > "$FICHIER_TMP"

# Vérification que le fichier temporaire n'est pas vide
if [ ! -s "$FICHIER_TMP" ]; then
    erreur "Le fichier temporaire est vide ou invalide après le filtrage."
fi

# Afficher le contenu du fichier temporaire pour le débogage
echo "Contenu du fichier temporaire (avant exécution du programme C) :"
cat "$FICHIER_TMP"

# Exécution du programme C
echo "Exécution du programme C..."
RESULTAT="tests/${TYPE_STATION}_${TYPE_CONSO}.csv"
$EXECUTABLE "$FICHIER_TMP" "$RESULTAT"

# Vérification si le programme C a réussi à générer le fichier
if [ $? -ne 0 ]; then
    erreur "Echec de l'exécution du programme C."
fi

# Vérifier si le fichier résultat a été créé
if [ ! -f "$RESULTAT" ]; then
    erreur "Le fichier de résultats ($RESULTAT) n'a pas été créé."
fi

echo "Traitement terminé. Les résultats sont disponibles dans $RESULTAT."
