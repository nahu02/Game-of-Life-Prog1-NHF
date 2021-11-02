#ifndef JATEKALLAS_H
#define JATEKALLAS_H

typedef struct Tabla{
    int** g; // grid (2d array)
    int sz, m; // szélesség, magasság
    SDL_Rect** rects; // A képernyőn megjelenített cellák listája (2d array)
}Tabla;

void init_tabla(Tabla* ujtabla, int szelesseg, int magassag); // Létrehozza, feltölti nullákkal (hasznélat után törlendő, memóriaszemét!!)
void destroy_tabla(Tabla* regitabla); // Kitörli a táblát
void flip(Tabla* t, int sor, int oszlop);
void uj_generacio(Tabla *t);

#endif