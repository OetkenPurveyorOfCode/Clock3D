set CFLAGS=-D_CRT_SECURE_NO_WARNINGS -DPLATFORM_DESKTOP
rem -fsanitize=undefined -fsanitize=address
echo %CC%
if %CC% EQU cl (
    set LINKFLAGS=gdi32.lib winmm.lib user32.lib shell32.lib
    set EXTRA_CFLAGS=
    set TARGET=-Fe:Clock3D_cl
    set OBJS=raudio.obj rcore.obj rglfw.obj rmodels.obj rshapes.obj rtext.obj rtextures.obj utils.obj
    set SUBSYSTEM=/link /SUBSYSTEM:windows
) else (
    set LINKFLAGS=-lwinmm -lgdi32 -luser32 -lshell32
    set EXTRA_CFLAGS=
    rem -Wall -Wextra -Wconversion
    set OBJS= *.o
    set TARGET=-o 3DClock_clang.exe
    set SUBSYSTEM=-mwindows
)

if not exist .\build (
    mkdir build
    pushd build
    call %CC% -c %CFLAGS% %EXTRA_CFLAGS% %USERFLAGS% -I../raylib/src/external/glfw/include ../raylib/src/*.c
    popd
)

pushd build
call %CC% %CFLAGS% %LINKFLAGS% %EXTRA_CFLAGS% %USERFLAGS% -I../raylib/src/ ../src/main.c %OBJS% %TARGET% %SUBSYSTEM%
popd

