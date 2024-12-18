#!/bin/bash

# Fonction d'aide
function afficher_aide {
    echo "Usage : ./c-wire.sh <chemin_du_fichier> <type_station> <type_consommateur> [id_centrale]"
    echo ""
    echo "type_station : hvb (HV-B), hva (HV-A), lv (Low Voltage)"
    echo "type_consommateur :"
    echo "  - comp (entreprises) pour hvb, hva, ou lv"
    echo "  - indiv (particuliers) pour lv uniquement"
    echo "  - all (tous) pour lv uniquement"
    echo "id_centrale : optionnel, identifiant de la centrale spécifique (entier)"
    exit 1
}

# Vérification si l'option d'aide est demandée
if [[ "$1" == "-h" ]]; then
    afficher_aide
fi

# Vérification de la présence de tous les paramètres obligatoires
if [ $# -lt 3 ]; then
    echo "Erreur : Trop peu de paramètres."
    afficher_aide
fi

# Paramètres
fichier_donnees="$1"
type_station="$2"
type_consommateur="$3"
id_centrale=$4

# Vérification des combinaisons autorisées
if [[ "$type_station" == "hvb" && "$type_consommateur" != "comp" ]] || 
   [[ "$type_station" == "hva" && "$type_consommateur" != "comp" ]] || 
   [[ "$type_station" == "lv" && "$type_consommateur" != "comp" && "$type_consommateur" != "indiv" && "$type_consommateur" != "all" ]] || 
   [[ "$type_station" != "lv" && "$type_consommateur" == "indiv" ]] || 
   [[ "$type_station" != "lv" && "$type_consommateur" == "all" ]]; then
    echo "Erreur : Combinaison type_station et type_consommateur invalide."
    afficher_aide
fi

# Vérification de l'existence du fichier d'entrée
if [ ! -f "$fichier_donnees" ]; then
    echo "Erreur : Le fichier '$fichier_donnees' n'existe pas."
    exit 1
fi

# Création des dossiers tmp et graphs si nécessaire
if [ ! -d "tmp" ]; then
    mkdir tmp
else
    rm -rf tmp/*
fi

if [ ! -d "graphs" ]; then
    mkdir graphs
fi

# Compilation du programme C si nécessaire
if [ ! -f "c-wire" ]; then
    echo "Le programme C n'existe pas. Compilation en cours..."
    make
    if [ $? -ne 0 ]; then
        echo "Erreur : La compilation a échoué."
        exit 1
    fi
fi

# Exécution du programme C pour générer le fichier de sortie
output_file="tmp/${type_station}_${type_consommateur}.csv"

# Lancer le programme C avec les paramètres et rediriger la sortie vers le fichier
start_time=$(date +%s)

./c-wire "$fichier_donnees" "$type_station" "$type_consommateur" "$id_centrale" > "$output_file"

end_time=$(date +%s)
execution_time=$((end_time - start_time))

# Affichage du temps d'exécution
echo "Le traitement a pris $execution_time secondes."

# Vérification si le fichier a été créé
if [ ! -f "$output_file" ]; then
    echo "Erreur : Le fichier de sortie '$output_file' n'a pas été créé."
    exit 1
fi

echo "Fichier de sortie généré : $output_file"

# Si l'option "lv all" est choisie, créer le fichier lv_all_minmax.csv
if [[ "$type_station" == "lv" && "$type_consommateur" == "all" ]]; then
    echo "Traitement des 10 postes LV avec la consommation la plus faible et la plus élevée..."

    # Appel du programme C pour générer le fichier lv_all_minmax.csv
    ./c-wire "$fichier_donnees" "lv" "all_minmax" "$id_centrale"

    # Vérification si le fichier a été créé
    lv_minmax_file="tmp/lv_all_minmax.csv"
    if [ ! -f "$lv_minmax_file" ]; then
        echo "Erreur : Le fichier 'lv_all_minmax.csv' n'a pas été généré."
        exit 1
    fi

    echo "Fichier de sortie minmax généré : $lv_minmax_file"
fi
