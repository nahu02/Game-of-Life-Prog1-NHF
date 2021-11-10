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
 * @param g int** | grid (2d array), 0/1 (halott/élő)
 * @param sz int | szélesség
 * @param m int | magasság
 * @param rects SDL_Rect** | A képernyőn megjelenített cellák listája (2d array)
 */
typedef struct Tabla{
    int** g; 
    int sz, m;
    SDL_Rect** rects;
}Tabla;

/**
 * @brief Linked list elem oszloppal és sorral
 * A megváltoztatott cellák listájának kezelésére
 * @param sor int
 * @param oszlop int
 * @param next OszlopSor_Lista_Elem* | következő listaelemre mutató pointer
 */
typedef struct OszlopSor_Lista_Elem{
    int sor, oszlop; 
    struct OszlopSor_Lista_Elem* next;
}OszlopSor_Lista_Elem;

/**
 * @brief Létrehozza, feltölti nullákkal a Tabla objektumot
 * A tábla sz és m paraméterei nem egyeznek a magassággal, hiszen extra nullásokból álló "szegélyt" kap köré
 * @warning Használat után törlendő memóriaszemetet hagy!!
 * @param ujtabla Tabla*
 * @param szelesseg int
 * @param magassag int
 */
void init_tabla(Tabla* ujtabla, int szelesseg, int magassag);

/**
 * @brief Felszabadítja a Tabla objektum memóriahelyét
 * @param regitabla Tabla*
 */
void destroy_tabla(Tabla* regitabla);

/**
 * @brief A megadott sorban és oszlopban lévő cella értékét megváltoztatja (Halott -> Élő, Élő -> Halott)
 * @param t Tabla*
 * @param sor int
 * @param oszlop int
 */
void flip(Tabla* t, int sor, int oszlop);

/**
 * @brief A szimulációt továbblépteti a következő állásba.
 * A szegély cellái nem változnak, ezek tulajdonképpen nem a szimuláció részei.
 * Linked list (első elemét) ad vissza, a megváltoztatott cellák koordinátájával.
 * @warning Felszabadítandó memóriaszemetet hagy (a linked listtel)!!
 * @param t Tabla*
 * @return OszlopSor_Lista_Elem* | a megváltoztatott cellák
 */
OszlopSor_Lista_Elem* uj_generacio(Tabla* t);

#endif