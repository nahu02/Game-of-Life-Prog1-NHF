gcc -c GoL_logics.c -o GoL_logics.o
gcc -c GoL_graphics.c -o GoL_graphics.o
gcc GoL_graphics.o GoL_logics.o -lmingw32 -lSDL2main -lSDL2_gfx -lSDL2_ttf -lSDL2_image -lSDL2  -o GoL.exe