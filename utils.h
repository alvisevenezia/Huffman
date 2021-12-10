#include <stdio.h>
#include <stdlib.h>

#ifndef HUFFMAN_UTILS_H
#define HUFFMAN_UTILS_H

//declaration des structures et fonctions utiles pour le programme

/**
 *
 * struture qui correspond a un neoud de l'arbre et contient:
 *
 *  - la valeur du coractère en cas de feuille
 *  - l'occurence du noeud ou du caractère en cas de feuille, cf sujet
 *  - l'adresse du fils gauche, null si feuille
 *  - l'adresse du fils droit, null si feuille
 */
typedef struct Arbre {
    unsigned char lettre;
    int occurence;
    struct Arbre* fg;
    struct Arbre* fd;
} Arbre;

/**
 *
 * structure qui correspond a un élément d'une liste d'Arbre et contient:
 *
 *  - l'adresse d'un arbre
 *  - l'adresse de l'élément suivant, null si fin de liste
 *
 */
typedef struct ListedArbre {
    Arbre* arbre;
    struct ListedArbre* suivant;
} ListedArbre;

/**
 *
 * fonction pour lire chaque octets d'un entier (4 octets) dans un fichier
 *
 * @param file fichier cible
 * @return
 */
int readInt(FILE* file);


/**
 *
 * fonction pour écrire chaque octets d'un entier (4 octets) dans un fichier cible
 *
 * @param file fichier cible
 * @param val entier a écrire
 */
void ecrireInt(FILE* file, unsigned int val);


/**
 *
 * fonction pour décaler de un bit vers la gauche les 32 premiers octet d'un tableau de 33 octets et incrémentation  de 1 de la valeur du dernier octet
 *
 * @param bits tableau de 33 octets de structure:
 *
 *                  - 32 octets de donnée
 *                  - 1 octet qui correspond au nombre de bits utiles
 *
 *
 */
void decalerBits(unsigned char* bits);


/**
 *
 * fonction qui revoie le maximum entre deux entiers
 *
 * @param a
 * @param b
 * @return
 */
int Max(int a, int b);


/**
 *
 * renvoie la somme des ocurences des deux fils d'un noeud de l'arbre
 *
 * @param a
 * @return
 */
int sommeFils(Arbre* a);


/**
 *
 * Vérifie le noeud actuel de l'arbre est un feuille
 *
 * @param a
 * @return
 */
int estFeuille(Arbre* a);


/**
 *
 * renvoie la hauteur d'un arbre
 *
 * @param a
 * @return
 */
int hauteur(Arbre* a);


/**
 *
 * Permet de créer un noeud de l'arbre
 *
 * @param lettre valeur du caratère si feuille
 * @param occurence occurence du noeud ou du caractère si feuille
 * @param fg adresse du fils gauche, null si feuille
 * @param fd adresse du fils droit, null si feuille
 * @return
 */
Arbre* creerArbre(char lettre, int occurence, Arbre* fg, Arbre* fd);


/**
 *
 * Permet de créer un élément de ListedArbre
 *
 * @param arbre adresse de l'arbre stocké
 * @return
 */
ListedArbre* creerChainonListedArbre(Arbre* arbre);


/**
 *
 * Permet de créer et d'insérer au bon endroit un élément dans une liste triée sur la base de la valeur d'occurence stockée dans l'arbre
 *
 * @param liste liste où insérer l'élement
 * @param arbre adresse de l'arbre du nouvelle élément
 * @return
 */
ListedArbre* InsererListedArbreTriee(ListedArbre* liste, Arbre* arbre);


/**
 *
 * Permet de convertir le tableau d'occurence en liste d'arbre
 *
 * @param tableau tableau de l'occurence des caractères dans le texte
 * @return
 */
ListedArbre* TabToListedArbreTriee(unsigned int tableau[256]);


/**
 *
 * Permet de convertir la liste d'arbre en un seul arbre selon l'algorithme de Huffmann
 *
 * @param listedA
 * @return
 */
Arbre* ListedArbreToArbre(ListedArbre* listedA);

/**
 *
 * fonction pour convertir l'arbre en tableau des valeurs d'encodage pour chaque caractère
 *
 * @param a racine de l'arbre
 * @param bin tableau pour stocker la valeur d'encodage de chaque caractère:
 *              ---> chaque "case" pointe vers un tableau de 33 octects avec:
 *                  - 32 octets de donnée
 *                  - 1 octet qui correspond au nombre de bits utiles
 *
 *
 * @param tampon tableau de 33 octets de même structure qui sert de tampon
 * @param depth profondeur actuelle dans l'arbre
 */
void ArbreToBin(Arbre* a, unsigned char ** bin, unsigned char* tampon, int depth);

#endif //HUFFMAN_UTILS_H
