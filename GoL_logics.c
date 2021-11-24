/**
 * @file GoL_logics.c
 * @author Nagy Ábel (CPD63P) (nagy.abel@edu.bme.hu)
 * @brief A játék fő logikáját leíró fájl.
 * A játéktábla manipulálásával foglalkozik.
 * @version 0.1
 * @date 2021-11-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "GoL_logics.h"
#include "src/debugmalloc.h"

/**
 * @brief A konzolra kiírja a Tabla objektumot (- a Halott, @ az Élő cella).
 * A függvény csak debugginghoz használandó, valós haszna nincs. 
 * @param t
 */
static void print_tabla(Tabla* t);

/**
 * @brief Megszámolja a Tabla objektumon az x,y (sor, oszlop) pont élő szomszédait.
 * @param t
 * @param x sor
 * @param y oszlop
 * @return szomszédok száma
 */
static int szomszedok_szama(Tabla* t, int x, int y);

void init_tabla(Tabla* ujtabla, int szelesseg, int magassag){
    szelesseg += 2; // A kívánt x*y tábla körül egy "láthatatlan" keret van, amin mindig halott cellák vannak
    magassag += 2;
    ujtabla -> sz = szelesseg;
    ujtabla -> m = magassag;
    ujtabla -> g = calloc(magassag, sizeof(int*));
    ujtabla -> rects = malloc(magassag * sizeof(SDL_Rect*));
    for(int sor=0; sor<magassag; sor++){
        ujtabla -> g[sor] = calloc(szelesseg, sizeof(int));
        ujtabla -> rects[sor] = malloc(szelesseg * sizeof(SDL_Rect));
    }
}

void destroy_tabla(Tabla* regitabla){
    if(regitabla != NULL){
        int magassag = regitabla -> m;
        for(int sor=0; sor<magassag; sor++){
            free(regitabla -> g[sor]);
            free(regitabla -> rects[sor]);
        }
        free(regitabla -> g);
        free(regitabla -> rects);
        regitabla = NULL;
    }
}

void print_tabla(Tabla* t){
    int magassag = t -> m;
    int szelesseg = t -> sz;
    for(int sor=1; sor<magassag-1; sor++){ // A szélek ne íródjanak ki, azok mindig 0-k; gyakorlatilag nem a játlkterület részei
        for(int oszlop=1; oszlop<szelesseg-1; oszlop++){
            printf("%c", t -> g[sor][oszlop]?'@':'-');
        }
        printf("\n");
    }
    printf("\n");
}

int szomszedok_szama(Tabla* t, int x, int y){
    int szum = 0;
    for(int sor=-1; sor<2; sor++)
        for(int oszlop=-1; oszlop<2; oszlop++)
            if(sor!=0 || oszlop!=0)
                szum += t -> g[x+sor][y+oszlop];
    return szum; 
}

void flip(Tabla* t, int sor, int oszlop){
    if(t->g[sor][oszlop])
        t->g[sor][oszlop] = 0;
    else
        t->g[sor][oszlop] = 1;
}

OszlopSor_Lista_Elem* uj_generacio(Tabla *t){ 
    int magassag = t -> m;
    int szelesseg = t -> sz;
    Tabla uj;
    OszlopSor_Lista_Elem *fej = NULL;

    init_tabla(&uj, szelesseg-2, magassag-2); // Sajnos muszáj kivonni kettőt, hiszen a szegélyt az init magától megalkotja
    for(int sor=0; sor<magassag; sor++){
        for(int oszlop=0; oszlop<szelesseg; oszlop++){
            if(sor != 0 && oszlop != 0 && sor != magassag-1 && oszlop != szelesseg-1){ // A szegélyek nem változnak, gyakorlatilag nem is a játékterület részei
                uj.g[sor][oszlop] = t->g[sor][oszlop];
                int szomszedok = szomszedok_szama(t, sor, oszlop);
                if (t->g[sor][oszlop] == 0) {
                    if (szomszedok == 3){
                        uj.g[sor][oszlop] = 1;
                        OszlopSor_Lista_Elem *legutobbi_valtozas;
                        legutobbi_valtozas = (OszlopSor_Lista_Elem*) malloc(sizeof(OszlopSor_Lista_Elem));
                        legutobbi_valtozas->next = fej;
                        legutobbi_valtozas->oszlop = oszlop;
                        legutobbi_valtozas->sor = sor;
                        fej = legutobbi_valtozas;
                    }
                }
                else{
                   if (szomszedok < 2 || szomszedok > 3){
                        uj.g[sor][oszlop] = 0;
                        OszlopSor_Lista_Elem *legutobbi_valtozas;
                        legutobbi_valtozas = (OszlopSor_Lista_Elem*) malloc(sizeof(OszlopSor_Lista_Elem));
                        legutobbi_valtozas->next = fej;
                        legutobbi_valtozas->oszlop = oszlop;
                        legutobbi_valtozas->sor = sor;
                        fej = legutobbi_valtozas;
                    }
                }
            }
        }
    }
    for(int sor=0; sor<magassag; sor++){
        for(int oszlop=0; oszlop<szelesseg; oszlop++){
            t->g[sor][oszlop] = uj.g[sor][oszlop];
        }
    }
    destroy_tabla(&uj);
    return fej;
}