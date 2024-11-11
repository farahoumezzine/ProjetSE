# ProjetSE

# Projet 02 - Téléchargement simultané de fichiers

Ce programme permet de télécharger plusieurs fichiers en parallèle 
en utilisant des threads. Chaque téléchargement est effectué via la bibliothèque libcurl, et la progression est affichée en pourcentage.

## Compilation
Pour compiler le programme, utilisez la commande suivante :

gcc -o downloader main.c -lcurl -lpthread
