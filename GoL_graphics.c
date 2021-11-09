/**
 * @file GoL_graphics.c
 * @author Nagy Ábel (CPD63P) (nagy.abel@edu.bme.hu)
 * @brief A program grafikáját szabályozzó fájl.
 * Felelős mindenért, ami a megjelenítés része.
 * @version 0.1
 * @date 2021-11-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include "src/debugmalloc.h"
#include "GoL_logics.h"

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
 * @brief Megvizsgálja hogy az (x,y) koordináta (általában a kurzor) az SDL_Rect elemen belül található -e (returns 1 or 0)
 * 
 * @param x const int
 * @param y const int
 * @param rect SDL_Rect
 * @return int | 1 or 0
 */
static int xy_in_rect(const int x, const int y, SDL_Rect rect);
/**
 * @brief Kiírja a megadott feliratot a megadott helyre a megadott betűtípussal.
 * A menü gombjainak feliratozásához használt.
 * @param renderer SDL_Renderer*
 * @param font_menu TTF_Font* | Egy betöltött betűtípus
 * @param hova SDL_Rect
 * @param str const char*
 */
static void szoveg_kiiro(SDL_Renderer *renderer, TTF_Font *font_menu, SDL_Rect hova, const char str[]);
/**
 * @brief kirajzol egy téglalapot és a megadott szöveget írja rá.
 * A menü gombjainak kirajzolásához használt.
 * @param renderer SDL_Renderer*
 * @param font_menu TTF_Font* | Egy betöltött betűtípus
 * @param gomb SDL_Rect
 * @param felirat const char*
 */
static void menu_rajzol_gomb(SDL_Renderer *renderer, TTF_Font *font_menu, SDL_Rect gomb, const char felirat[]);
/**
 * @brief kirajzolja egy Tábla objektum g gridje szerinti megadott (sor, oszlop) cellát a képernyőre.
 * Frissíti a renderert.
 * @param renderer SDL_Renderer*
 * @param t Tabla*
 * @param sor int
 * @param oszlop int
 */
static void jatek_rajzol_cella(SDL_Renderer *renderer, Tabla *t, int sor, int oszlop);
/**
 * @brief Egy megadott felületet letakar és rárajzolja a játékterület összes celláját.
 * Fontos, hogy nem az egész képernyőre raajzol ki, hanem csak a megadott területre.
 * Elég lassú lehet.
 * @param renderer SDL_Renderer*
 * @param hova SDL_Rect
 * @param t Tabla*
 */
static void jatek_kirajzol(SDL_Renderer *renderer, SDL_Rect hova, Tabla *t);
/**
 * @brief Ha az (x,y) koordináta egy cella belsejében van, megváltoztatja annak állapotát.
 * A s_jatek állapotban használatos, kattintás ellenőrzésére.
 * @param env Ablak_info*
 * @param t Tabla*
 * @param x int
 * @param y int
 */
static void jatek_kattint(Ablak_info *env, Tabla *t, int x, int y);
/**
 * @brief A szimulációt a következő állapotra lápteti.
 * Ki is rajzolja a változásokat.
 * @param env Ablak_info*
 * @param t Tabla*
 */
static void jatek_nextgen(Ablak_info *env, Tabla *t);

/**
 * @brief Inicializálja az SDL-t, betölti az alapállapotokat az Ablak_info objektumba.
 * Figyel a lehetséges hibákra, és hibaüzenettel kilép, ha fellépnek.
 * @param env Ablak_info*
 */
void sdl_init(Ablak_info *env);
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

void sdl_init(Ablak_info *env){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        SDL_Log("Hiba az SDL inditasaban: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow("Abel's Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, env->width_screen, env->height_screen, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    env->renderer = renderer;
    SDL_RenderClear(env->renderer);
    TTF_Init();
}

int main(void){
    Ablak_info env = {NULL, s_menu, 800, 600};
    sdl_init(&env);

    TTF_Font *font_menu = TTF_OpenFont("C:/Windows/Fonts/OCRAEXT.TTF", 54);
    if (!font_menu) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    Harom_hely gombok_helye;
    Tabla t;

    menu(&env, font_menu, &gombok_helye);
 
    SDL_Event ev;
    while (SDL_WaitEvent(&ev) && ev.type != SDL_QUIT) { 
        switch(ev.type){ 
            case SDL_WINDOWEVENT:
                if(ev.window.event == SDL_WINDOWEVENT_RESIZED){
                    env.width_screen = ev.window.data1;
                    env.height_screen = ev.window.data2;
                    switch(env.state){
                        case s_menu:
                            menu(&env, font_menu, &gombok_helye);
                            break;
                        case s_tabla_meret:
                            tabla_meret(&env, &t);
                            break;
                        case s_jatek:
                            jatek(&env, &t);
                            break;
                        case s_betolt:
                            // betolt(&env);
                            break;
                        case s_sugo:
                            // sugo(&env);
                            break;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (ev.button.button == SDL_BUTTON_LEFT) {
                    const int x = ev.button.x, y = ev.button.y;
                    if(env.state == s_menu){
                        if( xy_in_rect(x, y, gombok_helye.j) ){
                            // Játek
                            tabla_meret(&env, &t);
                            
                        }
                        else if ( xy_in_rect(x, y, gombok_helye.b) ){
                            // Betölt
                        }
                        else if ( xy_in_rect(x, y, gombok_helye.s) ){
                            // Súgó
                        }
                    }
                    else if(env.state == s_jatek){
                        jatek_kattint(&env, &t, x, y);
                    }
                }
                break;
            case SDL_KEYDOWN:
                    if (                      ev.key.keysym.sym == SDLK_ESCAPE) {menu(&env, font_menu, &gombok_helye);}
                    if (env.state == s_jatek & ev.key.keysym.sym == SDLK_SPACE ) {jatek_nextgen(&env, &t);} // Ideiglenes, a későbbiekben gomb lesz a grafikai felületen (bárlehet hogy kényelmi szempontből ez is marad)
                break;
        }
    }
    
    destroy_tabla(&t);
    TTF_CloseFont(font_menu);
    SDL_Quit();
    return 0;
}

int xy_in_rect(const int x, const int y, SDL_Rect rect){
    return ( (( x > rect.x ) && ( x < rect.x + rect.w ) && ( y > rect.y ) && ( y < rect.y + rect.h )) );
}

void szoveg_kiiro(SDL_Renderer *renderer, TTF_Font *font_menu, SDL_Rect hova, const char str[]){
    SDL_Surface *felirat = NULL;
    SDL_Texture *felirat_t = NULL;
    SDL_Color szoveg = {155, 255, 61};
    felirat = TTF_RenderUTF8_Blended(font_menu, str, szoveg);
    felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
    hova.x += (hova.w - felirat->w) / 2;
    hova.y += (hova.h - felirat->h) / 2;
    hova.w = felirat->w;
    hova.h = felirat->h;
    SDL_RenderCopy(renderer, felirat_t, NULL, &hova);
    SDL_FreeSurface(felirat);
    SDL_DestroyTexture(felirat_t);
}

void menu_rajzol_gomb(SDL_Renderer *renderer, TTF_Font *font_menu, SDL_Rect gomb, const char felirat[]){
    boxRGBA(renderer, gomb.x, gomb.y, gomb.x + gomb.w, gomb.y + gomb.h, 77, 128, 31, 127);
    szoveg_kiiro(renderer, font_menu, gomb, felirat);
}

void menu(Ablak_info *env, TTF_Font *font_menu, Harom_hely *gombok_helye){
    env->state = s_menu;
    SDL_RenderClear(env->renderer);

    const double width_gomb = env->width_screen*4/6, height_gomb = env->height_screen/7;

    gombok_helye->j.x = width_gomb/4;  gombok_helye->j.y = 1*height_gomb;  gombok_helye->j.w = width_gomb;  gombok_helye->j.h = height_gomb;
    gombok_helye->b.x = width_gomb/4;  gombok_helye->b.y = 3*height_gomb;  gombok_helye->b.w = width_gomb;  gombok_helye->b.h = height_gomb;
    gombok_helye->s.x = width_gomb/4;  gombok_helye->s.y = 5*height_gomb;  gombok_helye->s.w = width_gomb;  gombok_helye->s.h = height_gomb;

    SDL_Rect jatek  = gombok_helye->j;
    SDL_Rect betolt = gombok_helye->b;
    SDL_Rect sugo   = gombok_helye->s;

    // Háttér
    boxRGBA(env->renderer, 0, 0, env->width_screen, env->height_screen, 17, 28, 7, 255);

    menu_rajzol_gomb(env->renderer, font_menu, jatek,  "JÁTÉK");

    menu_rajzol_gomb(env->renderer, font_menu, betolt, "BETÖLTÉS");

    menu_rajzol_gomb(env->renderer, font_menu, sugo,   "SÚGÓ");

    SDL_RenderPresent(env->renderer);
}

void tabla_meret(Ablak_info *env, Tabla *t){
    env->state = s_tabla_meret;
    SDL_RenderClear(env->renderer);

    init_tabla(t, 15, 10);

    jatek(env, t);
}

void jatek_rajzol_cella(SDL_Renderer *renderer, Tabla *t, int sor, int oszlop){
    SDL_Rect hova = t->rects[sor][oszlop];
    if(t->g[sor][oszlop]){
        boxRGBA(renderer, hova.x, hova.y, hova.x + hova.w, hova.y + hova.h, 155, 255, 61, 255);
    }
    else{
        boxRGBA(renderer, hova.x, hova.y, hova.x + hova.w, hova.y + hova.h, 17, 28, 7, 255);
        rectangleRGBA(renderer, hova.x, hova.y, hova.x + hova.w, hova.y + hova.h, 155, 255, 61, 255);
    }
    SDL_RenderPresent(renderer);
}


void jatek_kirajzol(SDL_Renderer *renderer, SDL_Rect hova, Tabla *t){
    int cella_m = hova.h/(t->m-2), cella_sz = hova.w/(t->sz-2);
    // A cellák legyenek négyzet alakúak, akkor is ha a kivetítő felület nem az
    if (cella_m < cella_sz) {cella_sz = cella_m; hova.x = (hova.w-(cella_sz*(t->sz-2)))/2;}
    else {cella_m = cella_sz; hova.y = (hova.h-(cella_m*(t->m-2)))/2;}

    for(int sor = 1; sor < (t->m-1); sor++){
        for (int oszlop = 1; oszlop < (t->sz-1); oszlop++){
            SDL_Rect cella = {hova.x + (oszlop-1)*cella_sz, hova.y + (sor-1)*cella_m, cella_sz, cella_m};
            t -> rects[sor][oszlop] = cella;
            jatek_rajzol_cella(renderer, t, sor, oszlop);
        }
    }
}

void jatek(Ablak_info *env, Tabla *t){
    env->state = s_jatek;
    SDL_RenderClear(env->renderer);

    // Háttér
    boxRGBA(env->renderer, 0, 0, env->width_screen, env->height_screen, 17, 28, 7, 255);

    SDL_Rect canvas = {0, 0, env->width_screen, env->height_screen};
    jatek_kirajzol(env->renderer, canvas, t);

}

void jatek_kattint(Ablak_info *env, Tabla *t, const int x, const int y){
    for(int sor = 1; sor < (t->m-1); sor++){
        for (int oszlop = 1; oszlop < (t->sz-1); oszlop++){
            if(xy_in_rect(x, y, t->rects[sor][oszlop])){
                // SDL_Log("Kattintva: (%d, %d), Ennek értéke:%d!\n", oszlop, sor, t->g[sor][oszlop]);
                flip(t, sor, oszlop);
                jatek_rajzol_cella(env->renderer, t, sor, oszlop);
                break;
            }
        }
    }
}

void jatek_nextgen(Ablak_info *env, Tabla *t){
    OszlopSor_Lista_Elem* fej = uj_generacio(t);
    for(OszlopSor_Lista_Elem* iter = fej; iter != NULL; iter = iter->next){
        jatek_rajzol_cella(env->renderer, t, iter->sor, iter->oszlop);
    }

    // free linked list
    OszlopSor_Lista_Elem* elozo = NULL;
    for(OszlopSor_Lista_Elem* iter = fej; iter != NULL; iter = iter->next){
        free(elozo);
        elozo = iter;
    }
    free(elozo);
}