#ifndef JATEKALLAS_H
#define JATEKALLAS_H

typedef struct Tabla{
    int** g; // grid (2d array)
    int sz, m; // szélesség, magasság
    SDL_Rect** rects; // A képernyőn megjelenített cellák listája (2d array)
}Tabla;

/**
 * @brief Linked list elem oszloppal és sorral
 * A megváltoztatott cellák listájának kezelésére
 * @param oszlop 
 * @param sor
 * @param next következő listaelemre mutató pointer
 */
typedef struct OszlopSor_Lista_Elem{
    int oszlop, sor; 
    struct OszlopSor_Lista_Elem* next;
}OszlopSor_Lista_Elem;

// Létrehozza, feltölti nullákkal (használat után törlendő, memóriaszemét!!)
void init_tabla(Tabla* ujtabla, int szelesseg, int magassag);

void destroy_tabla(Tabla* regitabla);

void flip(Tabla* t, int sor, int oszlop);

/**
 * @brief A szimulációt továbblépteti a következő állásba.
 * Linked listed ad vissza, a megváltoztatott cellák koordinátájával.
 * @warning Felszabadítandó memóriaszemetet hagy!
 * @param t 
 * @return OszlopSor_Lista_Elem* 
 */
OszlopSor_Lista_Elem* uj_generacio(Tabla *t);

#endif