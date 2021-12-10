#include "utils.h"

int readInt(FILE* file) {

    int qbyte = fgetc(file);
    int tbyte = fgetc(file);
    int sbyte = fgetc(file);
    int fbyte = fgetc(file);

    return fbyte + (sbyte << 8) + (tbyte << 16) + (qbyte << 24);
}

/*----------------------------------------------*/

void decalerBits(unsigned char* bits){

    for(int id = 0; id < 31 ;id++) {

        unsigned char head = bits[id + 1] & 128;
        bits[id] <<= 1;
        if (head != 0) bits[id] |= 1;
    }
    bits[31] <<= 1;
    bits[32]++;
}

/*----------------------------------------------*/

void ecrireInt(FILE* file, unsigned int val) {

    int fbyte = (int)val & 0xFF;
    int sbyte = (int)(val >> 8) & 0xFF;
    int tbyte = (int)(val >> 16) & 0xFF;
    int qbyte = (int)(val >> 24) & 0xFF;

    fputc(qbyte, file);
    fputc(tbyte, file);
    fputc(sbyte, file);
    fputc(fbyte, file);
}

/*----------------------------------------------*/

int Max(int a, int b) {
    return (a < b ? b : a);
}

/*----------------------------------------------*/

int sommeFils(Arbre* a) {
    return a->fg->occurence + a->fd->occurence;
}

/*----------------------------------------------*/

int estFeuille(Arbre* a) {
    return !(a->fg || a->fd);
}


/*----------------------------------------------*/

int hauteur(Arbre* a) {
    if (!a) return -1;
    if (estFeuille(a)) return 0;
    return 1 + Max(hauteur(a->fg), hauteur(a->fd));
}

/*----------------------------------------------*/

Arbre* creerArbre(char lettre, int occurence, Arbre* fg, Arbre* fd) {

    Arbre* a = malloc(sizeof(Arbre));

    a->lettre = (unsigned char)lettre;
    a->occurence = occurence;
    a->fg = fg;
    a->fd = fd;

    return a;
}

/*----------------------------------------------*/

ListedArbre* creerChainonListedArbre(Arbre* arbre) {

    ListedArbre* nouveau = malloc(sizeof(Arbre));
    nouveau->arbre = arbre;
    nouveau->suivant = NULL;

    return nouveau;
}

/*----------------------------------------------*/

ListedArbre* InsererListedArbreTriee(ListedArbre* liste, Arbre* arbre) {

    ListedArbre* p = creerChainonListedArbre(arbre);
    ListedArbre* tampon = liste;

    if (liste == NULL) return p;

    if (liste->arbre->occurence > arbre->occurence) {

        p->suivant = liste;
        return p;
    }

    while (tampon->suivant != NULL) {

        if (tampon->suivant->arbre->occurence > arbre->occurence) {
            p->suivant = tampon->suivant;
            tampon->suivant = p;
            return liste;
        }

        tampon = tampon->suivant;
    }

    tampon->suivant = p;

    return liste;
}

/*----------------------------------------------*/

ListedArbre* TabToListedArbreTriee(unsigned int tableau[256]) {

    ListedArbre* liste = NULL;

    for (int id = 0; id < 256; id++) {
        if (tableau[id]) liste = InsererListedArbreTriee(liste, creerArbre((char)id, (int)tableau[id], NULL, NULL));
    }

    return liste;
}

/*----------------------------------------------*/

Arbre* ListedArbreToArbre(ListedArbre* listedA) {

    Arbre* arbre;

    while (listedA->suivant != NULL) {
        ListedArbre* premier = listedA;
        ListedArbre* second = listedA->suivant;

        arbre = creerArbre(0, ((premier->arbre->occurence) + (second->arbre->occurence)), premier->arbre, second->arbre);
        listedA = second->suivant;
        free(premier);
        free(second);
        listedA = InsererListedArbreTriee(listedA, arbre);
    }

    return arbre;
}

/*----------------------------------------------*/

void ArbreToBin(Arbre* a, unsigned char ** bin, unsigned char* tampon, int depth) {

    if (estFeuille(a)) {
        for(int i = 0; i < 33;i++) {
            bin[a->lettre][i] = tampon[i];
        }
    }
    else {

        decalerBits(tampon);

        unsigned char buffer_save[33];
        for(int i = 0; i < 33;i++) {
            buffer_save[i] = tampon[i];
        }

        ArbreToBin(a->fg, bin, tampon, depth + 1);

        for(int i = 0; i < 33;i++) {
            tampon[i] = buffer_save[i];
        }

        tampon[31] |= 1;

        ArbreToBin(a->fd, bin, tampon, depth + 1);
    }
}