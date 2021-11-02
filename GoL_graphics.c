#include <stdio.h>
#include <stdlib.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include "GoL_logics.h"

typedef enum Allapot{
    s_menu,         // A menü a három gombbal
    s_tabla_meret,  // A tábla méretének megadása a játék gombra kattintás után
    s_rajz,         // A játéktábla, mikor a szimuláció nem fut
    s_szimulacio,   // A szimuláció fut
    s_betolt,       // A mentés választó menü, a betöltés gomb megnyomása után
    s_sugo          // A súgó, a súgó gomb megnyomása után
}Allapot;

typedef struct Ablak_info{
    SDL_Renderer *renderer;
    Allapot state;
    int width_screen, height_screen;
}Ablak_info;

// 3 SDL_Rect, amik a renderer-n belül megmondják hol vannak a gombok (j-játék, b-betölt, s-súgó)
typedef struct Harom_hely{
    SDL_Rect j, b, s;
}Harom_hely;

static int xy_in_rect(const int x, const int y, SDL_Rect rect); // Megvizsgálja hogy a kurzor (x,y) az SDL_Rect elemen belül található -e (returns 1 or 0)
static void rajz_kattint(Ablak_info *env, Tabla *t, int x, int y);


void menu       (Ablak_info *env, TTF_Font *font_menu, Harom_hely *gombok_helye);
void tabla_meret(Ablak_info *env, Tabla *t);
void rajz       (Ablak_info *env, Tabla *t);

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
                        case s_rajz:
                            rajz(&env, &t);
                            break;
                        case s_szimulacio:
                            // szimulacio(&env);
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
                            SDL_Log("Jatek!\n");
                            tabla_meret(&env, &t);
                            
                        }
                        else if ( xy_in_rect(x, y, gombok_helye.b) ){
                            // Betölt
                            SDL_Log("Betolt!\n");
                        }
                        else if ( xy_in_rect(x, y, gombok_helye.s) ){
                            // Súgó
                            SDL_Log("Sugo!\n");
                        }
                    }
                    else if(env.state == s_rajz){
                        rajz_kattint(&env, &t, x, y);
                    }
                }
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

void menu_felirat_gomb(SDL_Renderer *renderer, TTF_Font *font_menu, SDL_Rect hova, const char str[]){
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
    menu_felirat_gomb(renderer, font_menu, gomb, felirat);
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

    rajz(env, t);
}

void rajz_rajzol_cella(SDL_Renderer *renderer, Tabla *t, int sor, int oszlop){
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


void tabla_kirajzol(SDL_Renderer *renderer, SDL_Rect hova, Tabla *t){
    int cella_m = hova.h/(t->m-2), cella_sz = hova.w/(t->sz-2);
    // A cellák legyenek négyzet alakúak, akkor is ha a kivetítő felület nem az
    if (cella_m < cella_sz) {cella_sz = cella_m; hova.x = (hova.w-(cella_sz*(t->sz-2)))/2;}
    else {cella_m = cella_sz; hova.y = (hova.h-(cella_m*(t->m-2)))/2;}

    for(int sor = 1; sor < (t->m-1); sor++){
        for (int oszlop = 1; oszlop < (t->sz-1); oszlop++){
            SDL_Rect cella = {hova.x + (oszlop-1)*cella_sz, hova.y + (sor-1)*cella_m, cella_sz, cella_m};
            t -> rects[sor][oszlop] = cella;
            rajz_rajzol_cella(renderer, t, sor, oszlop);
        }
    }
}

void rajz(Ablak_info *env, Tabla *t){
    env->state = s_rajz;
    SDL_RenderClear(env->renderer);

    // Háttér
    boxRGBA(env->renderer, 0, 0, env->width_screen, env->height_screen, 17, 28, 7, 255);

    SDL_Rect canvas = {0, 0, env->width_screen, env->height_screen};
    tabla_kirajzol(env->renderer, canvas, t);

}

void rajz_kattint(Ablak_info *env, Tabla *t, const int x, const int y){
    for(int sor = 1; sor < (t->m-1); sor++){
        for (int oszlop = 1; oszlop < (t->sz-1); oszlop++){
            if(xy_in_rect(x, y, t->rects[sor][oszlop])){
                SDL_Log("Kattintva: (%d, %d), Ennek értéke:%d!\n", oszlop, sor, t->g[sor][oszlop]);
                flip(t, sor, oszlop);
                rajz_rajzol_cella(env->renderer, t, sor, oszlop);
                break;
            }
        }
    }
}