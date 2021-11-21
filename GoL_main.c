/**
 * @file GoL_main.c
 * @author Nagy Ábel (CPD63P) (nagy.abel@edu.bme.hu)
 * @brief A program futtatásához szükséges main loop fájlaja.
 * @version 0.3
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

int main(void){
    Ablak_info env = {NULL, s_menu, 800, 600, NULL};
    sdl_init(&env);

    TTF_Font *font_menu = TTF_OpenFont("C:/Windows/Fonts/OCRAEXT.TTF", 54);
    if (!font_menu) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    Harom_hely gombok_helye;
    Ikonok_hely ikonok_helye;
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
                            jatek(&env,&t);
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
                        if(jatek_kattint(&env, &t, x, y)){
                            menu(&env, font_menu, &gombok_helye);
                            destroy_tabla(&t);
                        }
                    }
                }
                break;
            case SDL_KEYDOWN:
                    if (                       ev.key.keysym.sym == SDLK_ESCAPE) {menu(&env, font_menu, &gombok_helye); destroy_tabla(&t);}
                    if (env.state == s_jatek & ev.key.keysym.sym == SDLK_SPACE ) {jatek_nextgen(&env, &t);} // Ideiglenes, a későbbiekben gomb lesz a grafikai felületen (bárlehet hogy kényelmi szempontből ez is marad)
                    if (env.state == s_jatek & ev.key.keysym.sym == SDLK_s     ) {jatek_mentes(&env, &t);} // Ideiglenes, a későbbiekben gomb lesz a grafikai felületen (bárlehet hogy kényelmi szempontből ez is marad)
                break;
        }
    }
    
    destroy_tabla(&t);
    TTF_CloseFont(font_menu);
    SDL_DestroyTexture(env.icons);
    SDL_Quit();
    return 0;
}