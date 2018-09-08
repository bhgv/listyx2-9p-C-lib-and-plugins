#!/bin/sh

export LUA=$PWD/lua-5.3.5
export ROOT=$PWD

mkdir lib
mkdir obj
mkdir obj/9infr
#mkdir ./test

gcc -c -g -O2 -fPIC -o ./obj/Posix.o -Ilibstyx -I9infr libstyx/Posix.c
gcc -c -g -O2 -fPIC -o ./obj/styxserver.o -Ilibstyx -I9infr libstyx/styxserver.c
ar cru ./lib/libstyx.a ./obj/Posix.o ./obj/styxserver.o


wget https://www.lua.org/ftp/lua-5.3.5.tar.gz

tar xzf lua-5.3.5.tar.gz
cd lua-5.3.5
make linux
cd ..

cp $LUA/src/liblua.a ./lib

cd obj/9infr
gcc -c -g -O2 -fPIC -I$ROOT/9infr $ROOT/9infr/*.c
ar cru $ROOT/lib/9infr.a *.o
cd $ROOT

gcc -c -g -O2 -fPIC -o ./obj/lstyx.o -Ilibstyx -I9infr -I$LUA/src luastyx/lstyx.c
gcc -c -g -O2 -fPIC -o ./obj/lstyx_cgi.o -Ilibstyx -I9infr -I$LUA/src luastyx/lstyx_cgi.c
gcc -c -g -O2 -fPIC -o ./obj/lstyx_file.o -Ilibstyx -I9infr -I$LUA/src luastyx/lstyx_file.c
gcc -c -g -O2 -fPIC -o ./obj/lstyx_rpc.o -Ilibstyx -I9infr -I$LUA/src luastyx/lstyx_rpc.c
gcc -c -g -O2 -fPIC -o ./obj/lstyx_util_file.o -Ilibstyx -I9infr -I$LUA/src luastyx/lstyx_util_file.c
gcc -c -g -O2 -fPIC -o ./obj/lstyx_util_cgi.o -Ilibstyx -I9infr -I$LUA/src luastyx/lstyx_util_cgi.c
gcc -g -O2 -shared -dynamic \
    -o ./lib/styx.so \
    ./obj/lstyx.o \
    ./obj/lstyx_cgi.o \
    ./obj/lstyx_file.o \
    ./obj/lstyx_rpc.o \
    ./obj/lstyx_util_file.o \
    ./obj/lstyx_util_cgi.o \
    ./lib/libstyx.a \
    ./lib/9infr.a \
    ./lib/liblua.a

cp ./lib/styx.so $LUA/src/lua ./test


