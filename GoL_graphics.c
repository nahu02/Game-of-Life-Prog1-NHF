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
#include <windows.h>
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
 * 
 * @param renderer
 * @param font Egy betöltött betűtípus
 * @param hova
 * @param str
 */
static void szoveg_kiiro(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect hova, const char str[]);
/**
 * @brief kirajzol egy téglalapot és a megadott szöveget írja rá.
 * Későbbiekben a kattintás ellenőrzéséhez ajánlott eltárolni a megadott SDL_Rect négyszöget.
 * @param renderer 
 * @param font Egy betöltött betűtípus, amivel a gombra a szöveg kerül.
 * @param gomb 
 * @param felirat
 */
static void rajzol_gomb(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect gomb, const char felirat[]);
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
 * @brief Futtatja a szimulációt megszakításig.
 * 
 * @param env 
 * @param t 
 */
static void jatek_futtat(Ablak_info *env, Tabla *t);
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

void szoveg_kiiro(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect hova, const char str[]){
    SDL_Surface *felirat = NULL;
    SDL_Texture *felirat_t = NULL;
    SDL_Color szoveg = {155, 255, 61};
    felirat = TTF_RenderUTF8_Blended_Wrapped(font, str, szoveg, hova.w);
    felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
    hova.x += (hova.w - felirat->w) / 2;
    hova.y += (hova.h - felirat->h) / 2;
    hova.w = felirat->w;
    hova.h = felirat->h;
    SDL_RenderCopy(renderer, felirat_t, NULL, &hova);
    SDL_FreeSurface(felirat);
    SDL_DestroyTexture(felirat_t);
}

void rajzol_gomb(SDL_Renderer *renderer, TTF_Font *font, SDL_Rect gomb, const char felirat[]){
    boxRGBA(renderer, gomb.x, gomb.y, gomb.x + gomb.w, gomb.y + gomb.h, 77, 128, 31, 127);
    szoveg_kiiro(renderer, font, gomb, felirat);
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

    rajzol_gomb(env->renderer, font_menu, jatek,  "JÁTÉK");

    rajzol_gomb(env->renderer, font_menu, betolt, "BETÖLTÉS");

    rajzol_gomb(env->renderer, font_menu, sugo,   "SÚGÓ");

    SDL_RenderPresent(env->renderer);
}

void tabla_meret(Ablak_info *env, TTF_Font *font_meret, Tabla *t){
    env->state = s_tabla_meret;

    //Háttér
    boxRGBA(env->renderer, 0, 0, env->width_screen, env->height_screen, 17, 28, 7, 255);

    SDL_Rect szoveg_helye = {env->width_screen/20, 0, 18*env->width_screen/20, env->height_screen/3};
    char szoveg[1000];
    snprintf(szoveg, sizeof(szoveg), "Üdv a Game of Life szimulációban! Kérlek add meg a kívánt méretét a játéktábládnak. (x, y)!\n\nA neked ajánlott maximális méret: %dx%d", env->width_screen/25, env->height_screen/25);
    szoveg_kiiro(env->renderer, font_meret, szoveg_helye, szoveg);

    char szel_c[4], mag_c[4];
    int szel = 0, mag = 0;
    SDL_Rect szel_r = {  env->width_screen/3 - 40, 5*env->height_screen/6, 70, 40};
    SDL_Rect mag_r  = {2*env->width_screen/3 - 40, 5*env->height_screen/6, 70, 40};
    SDL_Color bg = {17, 28, 7}, fg = {155, 255, 61};
    SDL_Rect szel_szov_helye = {0, 2*env->height_screen/3, env->width_screen/2, env->height_screen/6};
    SDL_Rect mag_szov_helye  = {env->width_screen/2, 2*env->height_screen/3, env->width_screen/2, env->height_screen/6};
    szoveg_kiiro(env->renderer, font_meret, szel_szov_helye, "Szélesség:");
    SDL_RenderPresent(env->renderer);
    while(szel == 0){
        input_text(szel_c, 3, szel_r, bg, fg, font_meret, env->renderer);
        szel = atoi(szel_c);
    }
    szoveg_kiiro(env->renderer, font_meret, mag_szov_helye, "Magasság:");
    SDL_RenderPresent(env->renderer);
    while(mag == 0){
        input_text(mag_c, 3, mag_r, bg, fg, font_meret, env->renderer);
        mag = atoi(mag_c);
    }

    init_tabla(t, szel, mag);
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
    env->ikonok_helye.h = ikon_kirazol(env, Home, env->width_screen - 69, 5                      );
    env->ikonok_helye.s = ikon_kirazol(env, Save, env->width_screen - 69, env->height_screen - 69);
    env->ikonok_helye.n = ikon_kirazol(env, Next, 5,                      env->height_screen - 69);
    env->ikonok_helye.p = ikon_kirazol(env, Play, 5,                      env->height_screen - (69 + 10 + 64)); // TODO

    SDL_Rect canvas = {(env->width_screen)/10, 0, (8*env->width_screen)/10, env->height_screen};
    boxRGBA(env->renderer, canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h, 17, 28, 7, 255);
    rectangleRGBA(env->renderer, canvas.x, canvas.y, canvas.x + canvas.w, canvas.y + canvas.h, 155, 255, 61, 255);
    jatek_kirajzol(env->renderer, canvas, t);

}

int jatek_kattint(Ablak_info *env, TTF_Font *font_mentes, Tabla *t, const int x, const int y){
    if(xy_in_rect(x, y, env->ikonok_helye.p)){
        jatek_futtat(env, t);
        return 0;
    }
    if(xy_in_rect(x, y, env->ikonok_helye.n)){
        jatek_nextgen(env, t);
        return 0;
    }
    if(xy_in_rect(x, y, env->ikonok_helye.s)){
        jatek_mentes(env, font_mentes, t);
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
    return 0;
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

void jatek_futtat(Ablak_info *env, Tabla *t){
    boxRGBA(env->renderer, env->ikonok_helye.p.x, env->ikonok_helye.p.y, env->ikonok_helye.p.x + env->ikonok_helye.p.w, env->ikonok_helye.p.y + env->ikonok_helye.p.h, 17, 28, 7, 255);
    ikon_kirazol(env, Pause, env->ikonok_helye.p.x, env->ikonok_helye.p.y);
    SDL_RenderPresent(env->renderer);
    int kilep = 0;
    while (!kilep){
        SDL_Event ev;
        SDL_WaitEvent(&ev);
        switch (ev.type) {
            case SDL_MOUSEBUTTONDOWN:
                if(ev.button.button == SDL_BUTTON_LEFT){
                    kilep = 1;
                }
                break;

            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE){
                    kilep = 1;
                }
                break;
            
            case SDL_WINDOWEVENT:
                if(ev.window.event == SDL_WINDOWEVENT_RESIZED){
                    SDL_PushEvent(&ev);
                    kilep = 1;
                }
                break;

            case SDL_QUIT:
                SDL_PushEvent(&ev);
                kilep = 1;
                break;
            
            default:
                SDL_PushEvent(&ev);
        }
        jatek_nextgen(env, t);
    }
    boxRGBA(env->renderer, env->ikonok_helye.p.x, env->ikonok_helye.p.y, env->ikonok_helye.p.x + env->ikonok_helye.p.w, env->ikonok_helye.p.y + env->ikonok_helye.p.h, 17, 28, 7, 255);
    ikon_kirazol(env, Play, env->ikonok_helye.p.x, env->ikonok_helye.p.y);
    SDL_RenderPresent(env->renderer);
}

void jatek_mentes(Ablak_info *env, TTF_Font *font_mentes, Tabla *t){
    char name[63] = "./saves/";

    // Háttér
    boxRGBA(env->renderer, 0, 0, env->width_screen, env->height_screen, 17, 28, 7, 220);

    char beolvasott[51];
    beolvasott[0] = '\0';
    SDL_Rect hova = {(env->width_screen-400)/2, (env->height_screen-40)/2, 400, 40};
    SDL_Color hatter = {17, 28, 7};
    SDL_Color szoveg = {155, 255, 61};
    int sikeres = input_text(beolvasott, 50, hova, hatter, szoveg, font_mentes, env->renderer);
    strcat(name, beolvasott);
    if(strlen(beolvasott) && sikeres){
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
    jatek(env, t);
}

int betolt_betoltes(Ablak_info *env, char *name, Tabla *t){
    
    char filename[63] = "./saves/";
    strcat(filename, name);
    strcat(filename, ".txt");
    FILE* fp = fopen(filename, "rt");
    if(fp == NULL){
        SDL_Log("Hiba a %s fajl megnyitasaban!\n", filename);
        return 0;
    }

    //fájl beolvasása
    char v[4];
    if (fscanf(fp, "%[^\n]", &v) == 0) {fclose(fp); return 0;}
    v[4] = '\0';
    if (strcmp(v, "0.1")) {fclose(fp); return 0;}
    int szel, mag, hiba;
    if (fscanf(fp, " %d%d%[^\n]", &szel, &mag, &hiba) != 2) {fclose(fp); return 0;}
    init_tabla(t, szel-2, mag-2);

    for (int sor = 0; sor < t->m; sor++) {
        for (int oszlop = 0; oszlop < t->sz; oszlop++) {
            int c;
            fscanf(fp, "%d", &c);
            // if (c != 0 || c != 1) {destroy_tabla(t); return 0;}
            t->g[sor][oszlop] = c;
        }
    }
    
    fclose(fp);
    jatek(env, t);
    return 1;
}

SDL_Rect ikon_kirazol(Ablak_info *env, Icon ikon, int x, int y){
    SDL_Rect honnan = { (ikon % 3) * 64, (ikon / 3) * 64, 64, 64 };
    SDL_Rect hova = { x, y, 64, 64 };
    SDL_RenderCopy(env->renderer, env->icons, &honnan, &hova);
    return hova;
}

int input_text(char *dest, size_t hossz, SDL_Rect teglalap, SDL_Color hatter, SDL_Color szoveg, TTF_Font *font, SDL_Renderer *renderer) {
    /* Ez tartalmazza az aktualis szerkesztest */
    char composition[SDL_TEXTEDITINGEVENT_TEXT_SIZE];
    composition[0] = '\0';
    /* Ezt a kirajzolas kozben hasznaljuk */
    char textandcomposition[hossz + SDL_TEXTEDITINGEVENT_TEXT_SIZE + 1];
    /* Max hasznalhato szelesseg */
    int maxw = teglalap.w - 2;
    int maxh = teglalap.h - 2;
 
    dest[0] = '\0';
 
    int enter = 0;
    int kilep = 0;
 
    SDL_StartTextInput();
    while (!kilep && !enter) {
        /* doboz kirajzolasa */
        boxRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, hatter.r, hatter.g, hatter.b, 255);
        rectangleRGBA(renderer, teglalap.x, teglalap.y, teglalap.x + teglalap.w - 1, teglalap.y + teglalap.h - 1, szoveg.r, szoveg.g, szoveg.b, 255);
        /* szoveg kirajzolasa */
        int w;
        strcpy(textandcomposition, dest);
        strcat(textandcomposition, composition);
        if (textandcomposition[0] != '\0') {
            SDL_Surface *felirat = TTF_RenderUTF8_Blended(font, textandcomposition, szoveg);
            SDL_Texture *felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
            SDL_Rect cel = { teglalap.x, teglalap.y, felirat->w < maxw ? felirat->w : maxw, felirat->h < maxh ? felirat->h : maxh };
            SDL_RenderCopy(renderer, felirat_t, NULL, &cel);
            SDL_FreeSurface(felirat);
            SDL_DestroyTexture(felirat_t);
            w = cel.w;
        } else {
            w = 0;
        }
        /* kurzor kirajzolasa */
        if (w < maxw) {
            vlineRGBA(renderer, teglalap.x + w + 2, teglalap.y + 2, teglalap.y + teglalap.h - 3, szoveg.r, szoveg.g, szoveg.b, 192);
        }
        /* megjeleniti a képernyon az eddig rajzoltakat */
        SDL_RenderPresent(renderer);
 
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
            /* Kulonleges karakter */
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    int textlen = strlen(dest);
                    do {
                        if (textlen == 0) {
                            break;
                        }
                        if ((dest[textlen-1] & 0x80) == 0x00)   {
                            /* Egy bajt */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0x80) {
                            /* Bajt, egy tobb-bajtos szekvenciabol */
                            dest[textlen-1] = 0x00;
                            textlen--;
                        }
                        if ((dest[textlen-1] & 0xC0) == 0xC0) {
                            /* Egy tobb-bajtos szekvencia elso bajtja */
                            dest[textlen-1] = 0x00;
                            break;
                        }
                    } while(1);
                }
                if (event.key.keysym.sym == SDLK_RETURN) {
                    enter = 1;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    kilep = 1;
                }
                break;
 
            /* A feldolgozott szoveg bemenete */
            case SDL_TEXTINPUT:
                if (strlen(dest) + strlen(event.text.text) < hossz) {
                    strcat(dest, event.text.text);
                }
 
                /* Az eddigi szerkesztes torolheto */
                composition[0] = '\0';
                break;
 
            /* Szoveg szerkesztese */
            case SDL_TEXTEDITING:
                strcpy(composition, event.edit.text);
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) 
                    if (!xy_in_rect(event.button.x, event.button.y, teglalap))
                        kilep = 1;
                break;

            case SDL_QUIT:
                /* visszatesszuk a sorba ezt az eventet, mert
                 * sok mindent nem tudunk vele kezdeni */
                SDL_PushEvent(&event);
                kilep = 1;
                break;
            
            default:
                SDL_PushEvent(&event);
        }
    }
 
    /* 1 jelzi a helyes beolvasast; = ha enter miatt allt meg a ciklus */
    SDL_StopTextInput();
    return enter;
}

int sugo_kattint(Ablak_info *env, const int x, const int y){
    if(xy_in_rect(x, y, env->ikonok_helye.h)){
        return 1;
    }
    return 0;
}

void sugo(Ablak_info *env, TTF_Font *font_sugo, SDL_Texture *kep){
    env->state = s_sugo;
    SDL_RenderClear(env->renderer);

    // Háttér
    boxRGBA(env->renderer, 0, 0, env->width_screen, env->height_screen, 17, 28, 7, 255);

    SDL_Rect szoveg_canvas, kep_canvas, rip_canvas;
    if(env->width_screen < 800){
        kep_canvas.x = 0;
        kep_canvas.w = 0;
        kep_canvas.h = 0;
        kep_canvas.y = 0;

        rip_canvas.x = 0;
        rip_canvas.w = 0;
        rip_canvas.h = 0;
        rip_canvas.y = 0;

        szoveg_canvas.x = 2;
        szoveg_canvas.y = 2;
        szoveg_canvas.w = env->width_screen - 4;
        szoveg_canvas.h = env->height_screen - 4;

    }
    else if(env->width_screen < 900){
        kep_canvas.x = 2*env->width_screen/3;
        kep_canvas.w = 90;
        kep_canvas.h = kep_canvas.w/(6/5);
        kep_canvas.y = env->height_screen-kep_canvas.h-5;

        rip_canvas.x = kep_canvas.x + kep_canvas.w + 10;
        rip_canvas.w = env->width_screen - rip_canvas.x;
        rip_canvas.h = kep_canvas.h;
        rip_canvas.y = kep_canvas.y;

        szoveg_canvas.x = 10;
        szoveg_canvas.y = 5;
        szoveg_canvas.w = env->width_screen- 20;
        szoveg_canvas.h = env->height_screen - 30;

    }
    else{
        kep_canvas.x = 10;
        kep_canvas.w = env->width_screen/3 - 10;
        kep_canvas.h = kep_canvas.w/(6/5);
        kep_canvas.y = (env->height_screen - kep_canvas.h)/2;

        rip_canvas.x = kep_canvas.x;
        rip_canvas.w = kep_canvas.w;
        rip_canvas.y = kep_canvas.y + kep_canvas.h + 10;
        rip_canvas.h = env->height_screen - rip_canvas.y;

        szoveg_canvas.x = env->width_screen/3 + 10;
        szoveg_canvas.y = 10;
        szoveg_canvas.w = env->width_screen - kep_canvas.w - 20;
        szoveg_canvas.h = env->height_screen - 20;
    }

    char szoveg[] = "Az életjátékot (angolul: The game of life) John Horton Conway, a Cambridge-i Egyetem matematikusa találta ki. Játékként való megnevezése megtévesztő lehet, mivel „nullszemélyes” játék; és a „játékos” szerepe mindössze annyi, hogy megad egy kezdőalakzatot, és azután csak figyeli az eredményt. Matematikai szempontból az ún. sejtautomaták közé tartozik.\n\nA négyzetrács mezőit celláknak, a színes (élő) cellákat sejteknek nevezzük. Egy cella környezete a hozzá legközelebb eső 8 mező (tehát a cellához képest „átlósan” elhelyezkedő cellákat is figyelembe vesszük, feltesszük hogy a négyzetrácsnak nincs széle). Egy sejt/cella szomszédjai a környezetében lévő sejtek. A játék körökre osztott, a kezdő állapotban tetszőleges számú (egy vagy több) cellába sejteket helyezünk. Ezt követően a játékosnak nincs beleszólása a játékmenetbe. Egy sejttel (cellával) egy körben a következő három dolog történhet:\n- A sejt túléli a kört, ha két vagy három szomszédja van.\n- A sejt elpusztul, ha kettőnél kevesebb (elszigetelődés), vagy háromnál több (túlnépesedés) szomszédja van.\n- Új sejt születik minden olyan cellában, melynek környezetében pontosan három sejt található.\n\n(Forrás: wikipedia.org)";
    char rip[] = "RIP\nJohn H. Conway";
    SDL_RenderCopy(env->renderer, kep, NULL, &kep_canvas);
    szoveg_kiiro(env->renderer, font_sugo, rip_canvas, rip);
    szoveg_kiiro(env->renderer, font_sugo, szoveg_canvas, szoveg);
    env->ikonok_helye.h = ikon_kirazol(env, Home, env->width_screen - 69, 5);
    SDL_RenderPresent(env->renderer);
}

int betolt_kattint(Ablak_info *env, Tabla *t, const int x, const int y){
    if(xy_in_rect(x, y, env->ikonok_helye.h)){
        return 1;
    }

    HANDLE find_h = NULL;
    WIN32_FIND_DATA file;
    char path[1023];
    char filename[255];
    char mappa[] = "./saves";

    sprintf(path, "%s/*.txt", mappa);

    if((find_h = FindFirstFile(path, &file)) == INVALID_HANDLE_VALUE){
        SDL_Log("Hiba a %s path-on.", mappa);
        exit(1);
    }

    SDL_Rect gombok[10];
    char filenames[10][255];
    int cnt = 0;
    do{
        if(strcmp(file.cFileName, ".") != 0 && strcmp(file.cFileName, "..") != 0){
            sprintf(filename, "%s", file.cFileName);
            filename[strlen(filename)-4] = '\0';
            gombok[cnt].x = 5;
            gombok[cnt].y = env->height_screen/12 + cnt*(env->height_screen/12 + env->height_screen/12/11);
            gombok[cnt].w = env->width_screen-10;
            gombok[cnt].h = env->height_screen/12;
            strcpy(filenames[cnt],filename);
        }
    } while(++cnt<10 && FindNextFile(find_h, &file));

    for(int i = 0; i<=cnt; i++){
        if(xy_in_rect(x, y, gombok[i])){
            betolt_betoltes(env, filenames[i], t);
            FindClose(find_h);
            return 0;
        }
    }

    FindClose(find_h);
    return 0;
}

void betolt(Ablak_info *env, TTF_Font *font_betolt){
    env->state = s_betolt;
    SDL_RenderClear(env->renderer);

    // Háttér
    boxRGBA(env->renderer, 0, 0, env->width_screen, env->height_screen, 17, 28, 7, 255);

    HANDLE find_h = NULL;
    WIN32_FIND_DATA file;
    char path[1023];
    char filename[255];
    char mappa[] = "./saves";

    sprintf(path, "%s/*.txt", mappa);

    if((find_h = FindFirstFile(path, &file)) == INVALID_HANDLE_VALUE){
        SDL_Log("Hiba a %s path-on.", mappa);
        exit(1);
    }

    int cnt = 0;
    do{
        if(strcmp(file.cFileName, ".") != 0 && strcmp(file.cFileName, "..") != 0){
            sprintf(filename, "%s", file.cFileName);
            filename[strlen(filename)-4] = '\0';
            SDL_Rect gomb = {5, env->height_screen/12 + cnt*(env->height_screen/12 + env->height_screen/12/11), env->width_screen-10, env->height_screen/12};
            rajzol_gomb(env->renderer, font_betolt, gomb, filename);
        }
    } while(++cnt<10 && FindNextFile(find_h, &file));

    char szoveg[] = "Kérlek válassz a mentések közül!";
    SDL_Rect szoveg_rect = {5, 5, env->width_screen-10, env->height_screen/12-10};
    szoveg_kiiro(env->renderer, font_betolt, szoveg_rect, szoveg);
    env->ikonok_helye.h = ikon_kirazol(env, Home, env->width_screen-69, 5);
    FindClose(find_h);
    SDL_RenderPresent(env->renderer);
}