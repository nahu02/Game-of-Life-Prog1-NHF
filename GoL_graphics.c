/**
 * @file GoL_graphics.c
 * @author Nagy Ábel (CPD63P) (nagy.abel@edu.bme.hu)
 * @brief A program grafikáját szabályozzó fájl.
 * Felelős mindenért, ami a megjelenítés része.
 * @version 0.4
 * @date 2021-11-20
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
#include <SDL2/SDL_image.h>
#include <math.h>
#include "src/debugmalloc.h"
#include "GoL_logics.h"
#include "GoL_graphics.h"

/**
 * @brief Enum az icons.png fájl ikonjaival.
 * A számozás megegyezik a képfájlban az ikonok sorrendjével.
 */
typedef enum Icon{
    Play, Pause, Next,
    Save, Home
} Icon;

/**
 * @brief Kiírja a megadott feliratot a megadott helyre a megadott betűtípussal.
 * A menü gombjainak feliratozásához használt.
 * @param renderer
 * @param font_menu Egy betöltött betűtípus
 * @param hova
 * @param str
 */
static void szoveg_kiiro(SDL_Renderer *renderer, TTF_Font *font_menu, SDL_Rect hova, const char str[]);
/**
 * @brief kirajzol egy téglalapot és a megadott szöveget írja rá.
 * A menü gombjainak kirajzolásához használt.
 * @param renderer 
 * @param font_menu Egy betöltött betűtípus
 * @param gomb 
 * @param felirat
 */
static void menu_rajzol_gomb(SDL_Renderer *renderer, TTF_Font *font_menu, SDL_Rect gomb, const char felirat[]);
/**
 * @brief Kirajzolja egy Tábla objektum g gridje szerinti megadott (sor, oszlop) cellát a képernyőre.
 * Frissíti a renderert.
 * @param renderer
 * @param t
 * @param sor
 * @param oszlop
 */
static void jatek_rajzol_cella(SDL_Renderer *renderer, Tabla *t, int sor, int oszlop);
/**
 * @brief Egy megadott felületet letakar és rárajzolja a játékterület összes celláját.
 * Fontos, hogy nem az egész képernyőre raajzol ki, hanem csak a megadott területre.
 * Elég lassú lehet.
 * @param renderer
 * @param hova
 * @param t
 */
static void jatek_kirajzol(SDL_Renderer *renderer, SDL_Rect hova, Tabla *t);
/**
 * @brief Kirajzolja a képernyő koordinátákkal megadott pontjára a kért ikont.
 * 
 * @param env 
 * @param ikon 
 * @param x 
 * @param y 
 * @return A kirajzolt gomb helye
 */
static SDL_Rect ikon_kirazol(Ablak_info *env, Icon ikon, int x, int y);



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
    SDL_Texture *icons = IMG_LoadTexture(renderer, "./src/icons.png");
    if (icons == NULL) {
        SDL_Log("Nem nyithato meg a kepfajl: %s\n", IMG_GetError());
        exit(1);
    }
    env->renderer = renderer;
    env->icons = icons;
    SDL_RenderClear(env->renderer);
    TTF_Init();
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

    init_tabla(t, 20, 20);

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
    if (cella_m < cella_sz) {cella_sz = cella_m;}
    else {cella_m = cella_sz;}

    // A játéktábla a megjelenitési felület közepén jelenjen meg
    int jatektabla_szelesseg = cella_sz*(t->sz-2);
    int jatektabla_magassag  = cella_m *(t->m-2 );
    hova.x += ((hova.w-(jatektabla_szelesseg))/2);
    hova.y += ((hova.h-(jatektabla_magassag ))/2);

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

    // Gombok
    // Home jobb felül, Save jobb alul, Next bal alul, Play/Pause bal alul a Next felett
    env->ikonok_helye.h = ikon_kirazol(env, Home, env->width_screen - 69, 5                      ); // TODO
    env->ikonok_helye.s = ikon_kirazol(env, Save, env->width_screen - 69, env->height_screen - 69);
    env->ikonok_helye.n = ikon_kirazol(env, Next, 5,                      env->height_screen - 69);
    env->ikonok_helye.p = ikon_kirazol(env, Play, 5,                      env->height_screen - (69 + 10 + 64)); // TODO

    SDL_Rect canvas = {(env->width_screen)/10, 0, (8*env->width_screen)/10, env->height_screen};
    boxRGBA(env->renderer, canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h, 17, 28, 7, 255);
    rectangleRGBA(env->renderer, canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h, 155, 255, 61, 255);
    jatek_kirajzol(env->renderer, canvas, t);

}

int jatek_kattint(Ablak_info *env, Tabla *t, const int x, const int y){
        if(xy_in_rect(x, y, env->ikonok_helye.p)){
            // Hát, ezt valahogy ki kéne még találni
            return 0;
        }
        if(xy_in_rect(x, y, env->ikonok_helye.n)){
            jatek_nextgen(env, t);
            return 0;
        }
        if(xy_in_rect(x, y, env->ikonok_helye.s)){
            jatek_mentes(env, t);
            return 0;
        }
        if(xy_in_rect(x, y, env->ikonok_helye.h)){
            return 1;
        }
    for(int sor = 1; sor < (t->m-1); sor++){
        for (int oszlop = 1; oszlop < (t->sz-1); oszlop++){
            if(xy_in_rect(x, y, t->rects[sor][oszlop])){
                // SDL_Log("Kattintva: (%d, %d), Ennek értéke:%d!\n", oszlop, sor, t->g[sor][oszlop]);
                flip(t, sor, oszlop);
                jatek_rajzol_cella(env->renderer, t, sor, oszlop);
                return 0;
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

void jatek_play_stop(Ablak_info *env, Tabla *t, int play_stop){
    while (!play_stop){
        jatek_nextgen(env, t);
        SDL_Delay(500);
    }
}

void jatek_mentes(Ablak_info *env, Tabla *t){
    char name[63] = "./saves/";

    strcat(name, "test"); // placeholder to be replaced by user input (max 50 char)
    
    strcat(name, ".txt");
    FILE* fp = fopen(name, "wt");
    if(fp == NULL){
        SDL_Log("Hiba a %s fajl megnyitasaban!\n", name); // placeholder for actual message to the usr
    }
    else{
        fprintf(fp, "0.1\n");
        fprintf(fp, "%d %d\n", t->sz, t->m);
        for(int sor = 0; sor < (t->m); sor++){
        for (int oszlop = 0; oszlop < (t->sz); oszlop++){
            fprintf(fp, "%d", t->g[sor][oszlop]);
            if (oszlop != (t->sz-1)){
                fputc(' ', fp);
            }
        }
        fputc('\n', fp);
        }
        fclose(fp);
    }
}

SDL_Rect ikon_kirazol(Ablak_info *env, Icon ikon, int x, int y){
    SDL_Rect honnan = { (ikon % 3) * 64, (ikon / 3) * 64, 64, 64 };
    SDL_Rect hova = { x, y, 64, 64 };
    SDL_RenderCopy(env->renderer, env->icons, &honnan, &hova);
    return hova;
}

