set -x
mkdir build
cd build
$CC -DPLATFORM_DESKTOP $USERFLAGS -O3 -I../raylib/src/external/glfw/include -I../raylib/src/ ../raylib/src/*.c ../src/main.c -lGL -lm -lpthread -ldl -lrt -lX11 -o Clock3D
cd ..
