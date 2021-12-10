#include "utils.h"

#ifndef HUFFMAN_MAIN_H
#define HUFFMAN_MAIN_H


//déclaration des fonctions du main.c
void encodeFile(const char* pathR, const char* pathW);
void decodeFile(const char* pathR, const char* pathW);
int main(int argc, char* argv[]);

#endif //HUFFMAN_MAIN_H
