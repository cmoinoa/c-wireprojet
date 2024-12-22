README pour le script c-wire.sh (shell)
Description:
Le script c-wire.sh permet de traiter un fichier de données et de générer des rapports concernant la consommation des centrales électriques. Il enregistre les résultats dans des fichiers CSV,
en prenant en compte différents types de stations (hvb, hva, lv) et de consommateurs (comp, indiv, all). Il inclut également la possibilité de filtrer par un identifiant spécifique de centrale.

Prérequis: 
Le programme C c-wire.c doit être présent dans le même répertoire que ce script ou accessible.
Le fichier de données à traiter doit être au format CSV.

Le script suit les étapes suivantes :
Il vérifie les paramètres fournis en ligne de commande.
Il vérifie l'existence du fichier d'entrée.
Il compile le programme C si nécessaire.
Il exécute le programme C pour générer le fichier de sortie CSV.
Si le type de station est lv et le type de consommateur est all, il génère également un fichier indiquant les 10 postes avec la consommation la plus faible et la plus élevée.

Syntaxe: 
./c-wire.sh <chemin_du_fichier> <type_station> <type_consommateur> [id_centrale] (optionnel)

Paramètres: 
chemin du fichier : Le chemin vers le fichier de données à traiter.

type station peut être l'un des suivants :hvb, hva; lv

type_consommateur : Le type de consommateur est :
comp : Consommation par les entreprises
indiv : Consommation par les particuliers
all : Consommation totale 

id_centrale : Un identifiant optionnel d'une centrale spécifique
Exemple :
./c-wire.sh c-wire_v00.dat hvb comp //affichera la conso des hvb

Option d'aide
Pour afficher l'aide du script, utilisez l'option -h :


Structure des dossiers:
tmp : Contient les fichiers temporaires générés, y compris les fichiers CSV de sortie.
graphs : Dossier réservé pour la memoire, inutile pour l'obtention des resultats

Compilation:
Le programme C c-wire.c sera automatiquement compilé si le fichier exécutable c-wire n'existe pas. Il est aussi possible de le compiler manuellement avec la commande make












README pour le programme c-wire.c
Description:
Le programme c-wire.c est utilisé pour traiter un fichier de données contenant des informations sur des stations électriques (de type HVB, HVA, LV) et calculer la consommation des entreprises et des particuliers (ou totale). Il génère un fichier CSV contenant les résultats triés par capacité de station.

Les étapes principales:
Création de stations : Le programme crée des stations électriques en utilisant les informations extraites du fichier de données d'entrée.
Insertion dans un AVL: Les stations sont insérées dans un AVL triée par leur capacité.
Validation des lignes : Le programme vérifie que chaque ligne du fichier de données respecte les critères définis en fonction des paramètres de la station et du type de consommateur.
Écriture des résultats : Les résultats sont écrits dans un fichier CSV, avec des colonnes pour l'ID de la station, sa capacité et la consommation totale.

Structure du code : 

creer_station : Crée une nouvelle station avec les informations données.
inserer_station_triee : Insère une station dans une liste chaînée triée par capacité.
ligne_valide : Vérifie si une ligne de données respecte les critères en fonction du type de station et de consommateur.
traiter_fichier : Lit le fichier de données ligne par ligne, valide chaque ligne et insère les stations valides dans la liste chaînée.
ecrire_entete : Écrit l'en-tête descriptif dans le fichier de sortie.
imprimer_resultats : Imprime les résultats dans le fichier CSV.


Format du fichier de données d'entrée: 
Le fichier de données doit être un fichier texte contenant des lignes séparées par des points-virgules (;). Chaque ligne doit avoir 8 colonnes, dont les valeurs suivantes :

-ID de la station
-Capacités et consommations des différentes catégories de consommateurs
-D'autres données spécifiques aux stations électriques.
-Les valeurs manquantes doivent être indiquées par des tirets (-).

Fichier de sortie:
Le programme génère un fichier CSV avec les résultats. Le nom du fichier est basé sur le type de station et de consommateur, ce fichier contient les colonnes suivantes :
Station : L'ID de la station.
Capacité : La capacité de la station.
Consommation : La consommation totale de la station, en fonction du type de consommateur (entreprises, particuliers, ou totale).

Compilation:
gcc -o c-wire c-wire.c










Répartitions des taches : a chaque Td d'info c'etait l'opportunité de tester sur les machines et de poser des questions a notre enseignante, sinon chaque jour on codait sur VS code de notre coté et mettions les resultats sur github pour en discuter sur Discord
Limitations fonctionelles : on a eu du mal a generer les fichiers de sortie, il y a toujoours 2 fonctionnalitées manquantes, la conso de chaque poste ou station ( notre C trie mal le fichier csv d'entrée et ne l'implemente pas assez bien dans l'AVL) et la fonction Lv minmax ne marche pas non plus.
