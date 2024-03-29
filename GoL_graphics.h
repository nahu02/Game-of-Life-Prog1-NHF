/**
 * @file GoL_graphics.h
 * @author Nagy Ábel (CPD63P) (nagy.abel@edu.bme.hu)
 * @brief A program megjelenítéséhez használt funkciókat és structokat leíró headerfájl.
 * @version 0.3
 * @date 2021-11-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "GoL_logics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

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
 * @brief 3 SDL_Rect, amik a rendereren belül megmondják hol vannak a gombok.
 * Az s_menu állapotban használatos.
 * @param j A 'Játék' gomb jelenlegi helye a képernyőn
 * @param b A 'Betölt' gomb jelenlegi helye a képernyőn
 * @param s A 'Súgó' gomb jelenlegi helye a képernyőn
 */
typedef struct Harom_hely{
    SDL_Rect j, b, s;
}Harom_hely;

/**
 * @brief 4 SDL_Rect, amik a rendereren belül megmondják hol vannak az ikonok.
 * Több ikont használó állapotban is használható, de csak az s_jatek állapotban lesz hasznos mind a 4.
 * @param p A 'Play', vagy 'Stop' ikon jelenlegi helye a képernyőn. Ez a kettő sosem jelenik meg egyszerre, viszont mindig ugyanazt a helyet veszik fel.
 * @param n A 'Next' ikon jelenlegi helye a képernyőn.
 * @param s A 'Save' ikon jelenlegi helye a képernyőn.
 * @param h A 'Home' ikon jelenlegi helye a képernyőn.
 */
typedef struct Ikonok_hely{
    SDL_Rect p, n, s, h;
}Ikonok_hely;

/**
 * @brief Az Ablakra vonatkozó minden alapvető tulajdonság.
 * 
 * @param renderer
 * @param state
 * @param width_screen
 * @param height_screen
 * @param icons
 * @param ikonok_helye
 */
typedef struct Ablak_info{
    SDL_Renderer *renderer;
    Allapot state;
    int width_screen, height_screen;
    SDL_Texture *icons;
    Ikonok_hely ikonok_helye;
}Ablak_info;

/**
 * @brief Inicializálja az SDL-t, betölti az alapállapotokat az Ablak_info objektumba.
 * Figyel a lehetséges hibákra, és hibaüzenettel kilép, ha fellépnek.
 * @param env
 */
void sdl_init(Ablak_info *env);
/**
 * @brief Megvizsgálja hogy az (x,y) koordináta (általában a kurzor) az SDL_Rect elemen belül található -e (returns 1 or 0)
 * 
 * @param x
 * @param y
 * @param rect
 * @return 1 or 0
 */
int xy_in_rect(const int x, const int y, SDL_Rect rect);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_menu-re.
 * Eltüntet bármit ami épp a képernyőn van és kirajzolja a főmenüt.
 * @param env
 * @param font_menu  Egy betöltött betűtípus
 * @param gombok_helye
 */
void menu(Ablak_info *env, TTF_Font *font_menu, Harom_hely *gombok_helye);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_tabla_meret-re.
 * Eltüntet bármit ami épp a képernyőn van és kirajzolja a tábla méretét kiválasztó felületet.
 * Inicializálja a Tabla objektumot a megadott adatokkal.
 * @param env
 * @param t
 */
void tabla_meret(Ablak_info *env, TTF_Font *font_meret, Tabla *t);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_jatek-ra.
 * Eltüntet bármit ami épp a képernyőn van és kirajzolja a játéktáblát.
 * @param env
 * @param t
 */
void jatek(Ablak_info *env, Tabla *t);
/**
 * @brief Ellenőrzi az (x,y) koordináta helyét.
 * Ha az egy cella belsejében van, megváltoztatja annak állapotát.
 * Ha az egy ikon belsejében van, meghívja az ahhoz a gombhoz tartozó utasítást.
 * A s_jatek állapotban használatos, kattintás ellenőrzésére.
 * A visszatérési érték 1, ha ki kell lépni a menübe.
 * @param env
 * @param font_mentes A mentés nevének megadásához használandó betöltött betűtípus.
 * @param t
 * @param x
 * @param y
 * @return alapesetben 0; 1, ha ki kell lépni a menübe
 */
int jatek_kattint(Ablak_info *env, TTF_Font *font_mentes, Tabla *t, int x, int y);
/**
 * @brief A szimulációt a következő állapotra lápteti.
 * Ki is rajzolja a változásokat.
 * @param env
 * @param t
 */
void jatek_nextgen(Ablak_info *env, Tabla *t);
/**
 * @brief A felhasználótól bekér egy nevet, majd elmenti a játéktér jelenlegi állapotát azzal a névvel a ./saves mappába.
 * Elenőrzi, hogy a név már foglalt -e, vagy tartalmaz -e invalid karaktereket. Szól a felhasználónak ha igen.
 * Szól ha nem sikerült a mentés.
 * @warning Léteznie kell egy ./saves mappának!
 * @param env 
 * @param font_mentes A mentés nevének megadásához használandó betöltött betűtípus.
 * @param t 
 */
void jatek_mentes(Ablak_info *env, TTF_Font *font_mentes, Tabla *t);
/**
 * @brief A name nevű fájlt beolvassa, ha létezik tartalmát megjeleníti.
 * Ha nem sikeres a beolvasás, 0-t ad vissza.
 * @warning A name max 50 karakter lehet!
 * @param env 
 * @param name A ./saves/ mappán belüli betöltendő fájl neve. (Max 50 karakter!)
 * @param t Egy üres, iniciálizatlan Tabla objektum.
 * @return 1 ha sikeres
 */
int betolt_betoltes(Ablak_info *env, char *name, Tabla *t);
/**
 * @brief Beolvas egy szöveget a billentyűzetről.
 * A kódrészlet az infoC oldalról származik apró módosításokkal.
 * @param dest A tömb, ahová a beolvasott szöveg kerül, \0-val a végén.
 * @param hossz A maximális hossz, ami beolvasható, \0-vel együtt.
 * @param teglalap A beolvasódoboz helye.
 * @param hatter A háttár színe.
 * @param szoveg A szöveg színe.
 * @param font A rajzoláshoz használt font.
 * @param renderer 
 * @return 1 ha sikerült, 0 ha nem sikerült a beolvasás
 */
int input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer);
/**
 * @brief Ellenőrzi az (x,y) koordináta helyét.
 * Ha az a home ikon belsejében van, 1 a visszatérési érték és ki kell lépni a főmenübe.
 * @param env
 * @param x
 * @param y
 * @return alapesetben 0; 1, ha ki kell lépni a menübe
 */
int sugo_kattint(Ablak_info *env, const int x, const int y);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_sugo-ra.
 * Letörli a megjelenítőt és kiírja a súgót. Megjeníti a képet.
 * @param env 
 * @param font_sugo A szöveg kiírásához használt font.
 * @param kep A megjelenítendő kép (John Conway-ról).
 */
void sugo(Ablak_info *env, TTF_Font *font_sugo, SDL_Texture *kep);
/**
 * @brief Ellenőrzi az (x,y) koordináta helyét.
 * Ha az egy gombon van, meghívja a gomb nevével a betolt_betoltes függvényt.
 * Ha az a home ikon belsejében van, 1 a visszatérési érték és ki kell lépni a főmenübe.
 * @param env 
 * @param t 
 * @param x 
 * @param y 
 * @return alapesetben 0; 1, ha ki kell lépni a menübe 
 */
int betolt_kattint(Ablak_info *env, Tabla *t, const int x, const int y);
/**
 * @brief Megváltoztatja az Ablak_info objektum state-jét s_betoltes-re.
 * Letörli a megjelenítőt és kirajzolja a ./saves mappában található első 10 mentés betöltésére való gombokat.
 * @param env 
 * @param font_betolt Az ebben az állapotban használandó betűtípus.
 */
void betolt(Ablak_info *env, TTF_Font *font_betolt);

#endif