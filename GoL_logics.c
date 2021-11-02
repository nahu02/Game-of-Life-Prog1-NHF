#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "GoL_logics.h"


static void print_tabla(Tabla* t); // Deprecated, used for debugging
static int szomszedok_szama(Tabla* t, int x, int y); // Megszámolja a t táblán az x,y pont élő szomszédait

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
    int magassag = regitabla -> m;
    for(int sor=0; sor<magassag; sor++){
        free(regitabla -> g[sor]);
        free(regitabla -> rects[sor]);
    }
    free(regitabla -> g);
    free(regitabla -> rects);
    regitabla = NULL;
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

void uj_generacio(Tabla *t){ 
    int magassag = t -> m;
    int szelesseg = t -> sz;
    Tabla uj;
    init_tabla(&uj, szelesseg-2, magassag-2); // Sajnos muszáj kivonni kettőt, hiszen a szegélyt az init magától megalkotja
    for(int sor=0; sor<magassag; sor++){
        for(int oszlop=0; oszlop<szelesseg; oszlop++){
            if(sor != 0 && oszlop != 0 && sor != magassag-1 && oszlop != szelesseg-1){ // A szegélyek nem változnak, gyakorlatilag nem is a játékterület részei
                uj.g[sor][oszlop] = t->g[sor][oszlop];
                int szomszedok = szomszedok_szama(t, sor, oszlop);
                if (t->g[sor][oszlop] == 0) {
                    if (szomszedok == 3){
                        uj.g[sor][oszlop] = 1;
                    }
                }
                else{
                   if (szomszedok < 2 || szomszedok > 3){
                        uj.g[sor][oszlop] = 0;
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

}