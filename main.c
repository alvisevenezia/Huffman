#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "main.h"


/**
 *
 * Programme de Louis-Alexandre LAGUET, Alec SOQUET et Antoine WARLET PRE ING2 Gr 3
 *
 * Aucune partie du code n'est un copier-coller de code déjà existant

 *
 *
 */



/**********************************************/
/******************* Codage ******************/
/********************************************/


/**
 * Cette fonction permet d'encoder un fichier via l'algorithme de Huffmann
 *
 *  - Declartion et initialisation des variables
 *  - Ouverture des fichiers cibles
 *  - Lecture du ficher a coder et comptage de la fréquence de chaque caractère
 *  - Ecriture en en-tête du ficher de sortie le nombre de caractère codé
 *  - Création de l'arbre
 *  - Convertion de l'arbre en un tableau de binaire correspondant au chemin dans l'arbre pour chaque caractère, cf sujet
 *  - Ecriture en en-tête du ficher de sortie le tableau de fréquence des caractères
 *  - Retour au début du fichier "source" et lecture de ce dernier un par un
 *  - Encodage de chaque caractère dans un octet tampon et écriture de ce dernier lorsqu'il est "plein"
 *  - Affichage de la taille des fichier de d'un ratio de compression (% de données par rapport au fichier "source")
 *
 * @param pathR chemin d'acces au fichier a coder
 * @param pathW chemin d'acces du fichier de sortie, le fichier n'a pas besoin d'exister
 */
void encodeFile(const char* pathR, const char* pathW) {


    //déclaration des variables utiles

    int c = 0;
    int pos = 0;
    int nboctet = 0;
    int size = 0;
    int taille = 0;
    unsigned char tampon = 0b00000000;
    unsigned char octet = 0x00;
    unsigned int ascii[256] = { 0x00 };

    // init buffer
    unsigned char* buffer = malloc(33 * sizeof(char));

    for(int i = 0; i < 33;i++) {
        buffer[i] = 0;
    }

    // init array
    unsigned char ** bin = malloc(256 * sizeof(char*));
    for(int i = 0; i < 256; i++) {
        bin[i] = malloc(33 * sizeof(char));
        for(int j = 0; j < 33;j++) {
            bin[i][j] = 0;
        }
    }

    //Ouverture des fichiers cibles
    FILE* toRead = fopen(pathR, "rb");
    FILE* toWrite = fopen(pathW, "wb");

    //Lecture du ficher a coder et comptage de la fréquence de chaque caractère
    while ((c = fgetc(toRead)) != EOF) {

        size++;
        ascii[(int)c]++;
    }

    //Ecriture en en-tête du ficher de sortie le nombre de caractère codé
    ecrireInt(toWrite, size);

    //Création de l'arbre
    ListedArbre* listedarbre = TabToListedArbreTriee(ascii);
    Arbre* arbre = ListedArbreToArbre(listedarbre);

    //Convertion de l'arbre en un tableau de binaire correspondant au chemin dans l'arbre pour chaque caractère, cf sujet
    ArbreToBin(arbre, bin, buffer, 0);

    //Ecriture en en-tête du ficher de sortie le tableau de fréquence des caractères
    for (int id = 0; id < 256; id++) {
        ecrireInt(toWrite, ascii[id]);
    }

    //Retour au début du fichier "source" et lecture de ce dernier un par un
    fseek(toRead, 0, SEEK_SET);

    while ((c = fgetc(toRead)) != -1) {

        //Encodage de chaque caractère dans un octet tampon et écriture de ce dernier lorsqu'il est "plein"

        nboctet = (bin[c][32] / 8) + 1;

        //on itère pour chaque caratère autant de fois que sa valeur dans notre tableau de binaire comporte d'octets utiles, cf utils.h
        for (int id = nboctet; id > 0; id--) {

            //mise en tampon de l'octect lu
            tampon = bin[c][32 - id];
            //on regarde si le caractère est codé sur plus de un octet:
            // - si oui on regarde si l'octet actuel est le "premier", dans ce cas là on lui
            //   associe une taille correspondant au nombre de bits utiles modulo 8, sinon on lui associe 8
            // - sinon on lui associe une taille égale au nombre de bits utiles (forcément inférieur à 8 codé sur un seul octect)
            taille = (nboctet == 1) ? bin[c][32] : (nboctet == id ? bin[c][32] %8 : 8);

            //on vérifie que la position d'écriture finale dans notre octect tampon est supérieuer à 8 après ajout des octects actuels

            if (pos + taille <= 8) {

                //si non on ajoute les bits utils à droite de l'octet à écrire
                octet = (octet << taille) | tampon;
                //on met a jour la position d'écriture dans l'octect à écrire
                pos = pos + taille;

                //si notre octect à écrire est remplit on l'écrit dans le ficher et on remet la position d'écriture à 0
                if (pos == 8) {

                    fputc(octet, toWrite);
                    pos = 0;
                }
            }
            else {

                //si oui on ecrit le maximum de bits utiles san faire "déborder" l'octet à écrire, on écrit de dernier dans le fichier
                //et on écrit les bits utils restant au début de l'octet à écrire
                int t = 8 - pos;
                octet = (octet << t) | (tampon >> (taille - t));
                fputc(octet, toWrite);

                octet = (0b11111111 >> (8 - taille + t)) & tampon;
                pos = taille - t;
            }
        }
    }

    //un fois le fichier lu, on complète le dernier octet a écrire avec des 0 et on l'écrit dans le fichier
    if (pos < 8) {

        octet = octet << (8 - pos);
        fputc(octet, toWrite);
    }

    //on récupère la taille du fichier codé
    int sizeCoded = ftell(toWrite);

    //on affiche les tailles des deux fichiers et un ratio de compression
    printf("%dKB ---> %dkB : %d %%\n",size/1000,sizeCoded/1000,(sizeCoded*100)/size);

    //on ferme les fichiers
    fclose(toWrite);
    fclose(toRead);

}

/**********************************************/
/****************** Décodage *****************/
/********************************************/
/**
 * Cette fonction permet de décoder un fichier via l'algorithme de Huffmann
 *
 *  - Declartion et initialisation des variables
 *  - Ouverture des fichiers cibles
 *  - Lecture du ficher à décoder et récupération du nombre de cractère a décodé et du tableau de fréquence des caractères
 *  - Création de l'arbre
 *  - Convertion de l'arbre en un tableau de binaire correspondant au chemin dans l'arbre pour chaque caractère, cf sujet
 *  - Decadage de chaque caractère et écriture dans le ficher de sortie
 *
 *
 * @param pathR
 * @param pathW
 */
void decodeFile(const char* pathR, const char* pathW) {

    //Declartion et initialisation des variables
    int pos_octet = 0;
    unsigned int ascii[256] = { 0 };
    int nocw = 0;
    unsigned char octet;
    unsigned char mask = 0b10000000;
    unsigned char tampon;
    unsigned char* buffer = malloc(33 * sizeof(char));

    for(int i = 0; i < 33;i++) {
        buffer[i] = 0;
    }
    unsigned char ** bin = malloc(256 * sizeof(char*));
    for(int i = 0; i < 256; i++) {
        bin[i] = malloc(33 * sizeof(char));
        for(int j = 0; j < 33;j++) {
            bin[i][j] = 0;
        }
    }

    //Ouverture des fichiers cibles
    FILE* f = fopen(pathR, "rb");
    FILE* toWrite = fopen(pathW, "wb");

    //Lecture du ficher à décoder et récupération du nombre de cractère a décodé et du tableau de fréquence des caractères
    int size = readInt(f);

    for (int id = 0; id < 256; id++) {

        ascii[id] = readInt(f);
    }

    //Création de l'arbre
    Arbre* a = ListedArbreToArbre(TabToListedArbreTriee(ascii));
    Arbre* posArbre = a;



    // Convertion de l'arbre en un tableau de binaire correspondant au chemin dans l'arbre pour chaque caractère, cf sujet
    ArbreToBin(a, bin, buffer, 0);
    int t = 0;

    //on itère tant que le bombre de caractère écrit est inférieur au nombre de caractères à décoder (nocw = Number Of Caratere Written)
    while (nocw < size) {

        //on récupère l'octect lu
        octet = getc(f);
        t++;

        //on itère pour chaque bit de l'octect en vérifiant que le bombre de caractère écrit est inférieur au nombre de caractères à décoder
        while (pos_octet < 8 && nocw < size) {

            //on traite les bits de gauche a droite
            tampon = (octet & (mask >> pos_octet));

            //on se déplace dans l'arbre
            if (tampon == 0 && posArbre->fg) {
                posArbre = posArbre->fg;
            }
            else if (posArbre->fd) {
                posArbre = posArbre->fd;
            }

            //on vérifie si un arrive à une feuille
            if (estFeuille(posArbre)) {


                //si oui on écrit la valeur de caractère décodé
                fputc(posArbre->lettre, toWrite);
                //on incrémente de 1 le nombre de caractères écrits
                nocw++;
                //on se replace a la racine de l'arbre
                posArbre = a;
            }

            //on incrémente de 1 la position dans l'octet
            pos_octet++;
        }

        //une fois fini avec l'octect actuel on remet la position dans l'octct a 0
        pos_octet = 0;
    }

    //on ferme les fichiers
    fclose(toWrite);
    fclose(f);
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
/* = = = = = = = = = = = = = = MAIN = = = = = = = = = = = = = = */
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
/**
 *
 * On vérifie le nomdre de caractère et si chaque caractère correpond à la valeur attendue
 *
 *
 *
 * @param argc nombre d'argument de la commande
 * @param argv valeur de chaque argument de la commande
 * @return
 */
int main(int argc, char* argv[]) {

    //On vérifie le nomdre de caractère est celui attenudu
    if(argc == 4){

        //on regarde ce que l'utilisateur veut faire
        if(strcmp(argv[1],"-c") == 0){

            //on vérifie que le fichier source existe et qu'on a les permissions pour le lire
            if(access(argv[2],R_OK)){

                printf("Impossible de lire le fichier %s\n",argv[2]);
                printf("Utiliser la fonction ./huffman -h pour plus d'informations\n");
            }
            //sinon on affiche un message d'erreur et on oriente vers la notice
            else {

                clock_t before = clock();
                encodeFile(argv[2],argv[3]);
                printf("Finis en: %lu ms\n",(clock() - before));
            }
        }
        else if(strcmp(argv[1],"-d") == 0) {

            //on vérifie que le fichier source existe et qu'on a les permissions pour le lire
            if(access(argv[2],R_OK)){

                printf("Impossible de lire le fichier %s\n",argv[2]);
                printf("Utiliser la commande ./huffman -h pour plus d'informations\n");
            }
            //sinon on affiche un message d'erreur et on oriente vers la notice
            else {
                clock_t before = clock();
                decodeFile(argv[2],argv[3]);
                printf("Finis en: %lu ms\n",(clock() - before));
            }
        }else {
            printf("Mauvaise utlisation de la commande, voir ./huffman -h\n");
        }
    }
    else if(argc == 2){

        //on vérifie la syntaxe de la commande
        if(strcmp(argv[1],"-h") == 0){

            printf("Utiliser la commande ./huffman -c <fichier a compresser> <fichier d'arrivee> pour compresser un fichier\n");
            printf("Utiliser la commande ./huffman -d <fichier a lire> <fichier d'arrivee> pour decompresser un fichier\n");

        }
        //si elle n'est pas bonne on affiche un message d'erreur et on oriente vers la notice
        else {
            printf("Mauvaise utlisation de la commande, voir ./huffman -h\n");
        }

    }
    //sinon on affiche un message d'erreur et on oriente vers la notice
    else {
        printf("Mauvaise utlisation de la commande, voir ./huffman -h\n");
    }

    return 0;
}