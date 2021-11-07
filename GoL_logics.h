#ifndef JATEKALLAS_H
#define JATEKALLAS_H

typedef struct Tabla{
    int** g; // grid (2d array)
    int sz, m; // szélesség, magasság
    SDL_Rect** rects; // A képernyőn megjelenített cellák listája (2d array)
}Tabla;

// Létrehozza, feltölti nullákkal (használat után törlendő, memóriaszemét!!)
void init_tabla(Tabla* ujtabla, int szelesseg, int magassag);

void destroy_tabla(Tabla* regitabla);

void flip(Tabla* t, int sor, int oszlop);

void uj_generacio(Tabla *t);

#endif