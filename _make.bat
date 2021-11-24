cd .\NHF\
gcc -c GoL_logics.c -o GoL_logics.o
gcc -c GoL_graphics.c -o GoL_graphics.o
gcc -c GoL_main.c -o  GoL_main.o
gcc GoL_main.o GoL_graphics.o GoL_logics.o -lmingw32 -lSDL2main -lSDL2_gfx -lSDL2_ttf -lSDL2_image -lSDL2 -o GoL.exe
:: Hogy ne nyisson meg konzolt:
:: gcc GoL_main.o GoL_graphics.o GoL_logics.o -lmingw32 -lSDL2main -lSDL2_gfx -lSDL2_ttf -lSDL2_image -lSDL2 -mwindows -o GoL.exe

.\GoL.exe