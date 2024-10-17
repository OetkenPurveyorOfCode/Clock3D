set CFLAGS=-D_CRT_SECURE_NO_WARNINGS -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2 -Os -DBAKE 
rem -Wall -Wextra -Wconversion 
mkdir build
call emcc %CFLAGS% -I./raylib/src/external/glfw/include ^
    -I./raylib/src/ ^
    ./raylib/src/rcore.c ./raylib/src/rshapes.c ^
    ./raylib/src/rtextures.c ./raylib/src/rtext.c ^
    ./raylib/src/utils.c ./raylib/src/rmodels.c ^
    ./raylib/src/raudio.c ^
    --shell-file ./src/shell.html ^
    -o ./build/index.html ^
    -s USE_GLFW=3 ^
    -s ASSERTIONS=1 ^
    -s TOTAL_MEMORY=536870912 ^
    -s ASYNCIFY ^
    ./src/main.c

