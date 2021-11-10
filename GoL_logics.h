/**
 * @file GoL_logics.h
 * @author Nagy Ábel (CPD63P) (nagy.abel@edu.bme.hu)
 * @brief A játék logikájában használt függvényeket és structokat leíró header fájl.
 * @version 0.1
 * @date 2021-11-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef GOL_LOGICS_H
#define GOL_LOGICS_H

/**
 * @brief A GoL játéktábla tárolására való struct
 * 
 * @param g grid (2d array), 0/1 (halott/élő)
 * @param sz szélesség
 * @param m magasság
 * @param rects A képernyőn megjelenített cellák listája (2d array)
 */
typedef struct Tabla{
    int** g; 
    int sz, m;
    SDL_Rect** rects;
}Tabla;

/**
 * @brief Linked list elem oszloppal és sorral
 * A megváltoztatott cellák listájának kezelésére
 * @param sor
 * @param oszlop
 * @param next következő listaelemre mutató pointer
 */
typedef struct OszlopSor_Lista_Elem{
    int sor, oszlop; 
    struct OszlopSor_Lista_Elem* next;
}OszlopSor_Lista_Elem;

/**
 * @brief Létrehozza, feltölti nullákkal a Tabla objektumot
 * A tábla sz és m paraméterei nem egyeznek a magassággal, hiszen extra nullásokból álló "szegélyt" kap köré
 * @warning Használat után törlendő memóriaszemetet hagy!!
 * @param ujtabla
 * @param szelesseg
 * @param magassag
 */
void init_tabla(Tabla* ujtabla, int szelesseg, int magassag);

/**
 * @brief Felszabadítja a Tabla objektum memóriahelyét
 * @param regitabla
 */
void destroy_tabla(Tabla* regitabla);

/**
 * @brief A megadott sorban és oszlopban lévő cella értékét megváltoztatja (Halott -> Élő, Élő -> Halott)
 * @param t
 * @param sor
 * @param oszlop
 */
void flip(Tabla* t, int sor, int oszlop);

/**
 * @brief A szimulációt továbblépteti a következő állásba.
 * A szegély cellái nem változnak, ezek tulajdonképpen nem a szimuláció részei.
 * Linked list (első elemét) ad vissza, a megváltoztatott cellák koordinátájával.
 * @warning Felszabadítandó memóriaszemetet hagy (a linked listtel)!!
 * @param t
 * @return a megváltoztatott cellák
 */
OszlopSor_Lista_Elem* uj_generacio(Tabla* t);

#endif