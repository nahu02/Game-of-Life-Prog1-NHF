/**
 * @file GoL_graphics.h
 * @author Nagy Ábel (CPD63P) (nagy.abel@edu.bme.hu)
 * @brief A program megjelenítéséhez használt funkciókat és structokat leíró headerfájl.
 * @version 0.1
 * @date 2021-11-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "GoL_logics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#ifndef GOL_GRAPHICS_H
#define GOL_GRAPHICS_H

/**
 * @brief A program jelenlegi állapotát leíró enum.
 */
typedef enum Allapot{
    /** A menü a három gombbal */
    s_menu,
    /** A tábla méretének megadása a játék gombra kattintás után */
    s_tabla_meret,
    /** A játéktábla */
    s_jatek,
    /** A mentés választó menü, a betöltés gomb megnyomása után */
    s_betolt,
    /** A súgó, a súgó gomb megnyomása után */
    s_sugo
}Allapot;

/**
 * @brief Az Ablakra vonatkozó minden alapvető tulajdonság.
 * 
 * @param renderer SDL_Renderer*
 * @param state Allapot
 * @param width_screen int
 * @param height_screen int
 */
typedef struct Ablak_info{
    SDL_Renderer *renderer;
    Allapot state;
    int width_screen, height_screen;
}Ablak_info;

/**
 * @brief 3 SDL_Rect, amik a renderer-n belül megmondják hol vannak a gombok.
 * Az s_menu állapotban használatos.
 * @param j SDL_Rect | A 'Játék' gomb jelenlegi helye a képernyőn
 * @param b SDL_Rect | A 'Betölt' gomb jelenlegi helye a képernyőn
 * @param s SDL_Rect | A 'Súgó' gomb jelenlegi helye a képernyőn
 */
typedef struct Harom_hely{
    SDL_Rect j, b, s;
}Harom_hely;

/**
 * @brief Inicializálja az SDL-t, betölti az alapállapotokat az Ablak_info objektumba.
 * Figyel a lehetséges hibákra, és hibaüzenettel kilép, ha fellépnek.
 * @param env Ablak_info*
 */
void sdl_init(Ablak_info *env);
/**
 * @brief Megvizsgálja hogy az (x,y) koordináta (általában a kurzor) az SDL_Rect elemen belül található -e (returns 1 or 0)
 * 
 * @param x const int
 * @param y const int
 * @param rect SDL_Rect
 * @return int | 1 or 0
 */
int xy_in_rect(const int x, const int y, SDL_Rect rect);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_menu-re.
 * Eltüntet bármit ami épp a képernyőn van és kirajzolja a főmenüt.
 * @param env Ablak_info*
 * @param font_menu TTF_Font | Egy betöltött betűtípus
 * @param gombok_helye Harom_hely*
 */
void menu(Ablak_info *env, TTF_Font *font_menu, Harom_hely *gombok_helye);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_tabla_meret-re.
 * Eltüntet bármit ami épp a képernyőn van és kirajzolja a tábla méretét kiválasztó felületet.
 * Inicializálja a Tabla objektumot a megadott adatokkal.
 * @param env Ablak_info*
 * @param t Tabla*
 */
void tabla_meret(Ablak_info *env, Tabla *t);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_jatek-ra.
 * Eltüntet bármit ami épp a képernyőn van és kirajzolja a játéktáblát.
 * @param env Ablak_info*
 * @param t Tabla*
 */
void jatek(Ablak_info *env, Tabla *t);
/**
 * @brief Ha az (x,y) koordináta egy cella belsejében van, megváltoztatja annak állapotát.
 * A s_jatek állapotban használatos, kattintás ellenőrzésére.
 * @param env Ablak_info*
 * @param t Tabla*
 * @param x int
 * @param y int
 */
void jatek_kattint(Ablak_info *env, Tabla *t, int x, int y);
/**
 * @brief A szimulációt a következő állapotra lápteti.
 * Ki is rajzolja a változásokat.
 * @param env Ablak_info*
 * @param t Tabla*
 */
void jatek_nextgen(Ablak_info *env, Tabla *t);

#endif