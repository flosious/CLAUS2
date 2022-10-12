#!/bin/bash
/home/florian/toolchains/mxe/usr/x86_64-w64-mingw32.static/qt5/bin/qmake
sed -i 's/`PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig pkg-config --cflags --libs gsl sqlite3`/`PKG_CONFIG_PATH=/home/florian/projects/libs/win64/gsl/lib/pkgconfig x86_64-w64-mingw32.static-pkg-config --cflags --libs gsl` `PKG_CONFIG_PATH=/home/florian/projects/libs/win64/sqlite3/lib/pkgconfig x86_64-w64-mingw32.static-pkg-config --cflags --libs sqlite3`/g' Makefile.*
#sed -i 's/pkgconfig gsl/pkgconfig x86_64-w64-mingw32.static-pkg-config --cflags --libs gsl/g' Makefile.*
sed -i 's/-Wall//g' Makefile.*
sed -i 's/-Wextra//g' Makefile.*
make -j 8
cp release/qtGUI.exe /home/florian/Sync/claus2/qtGUI2.exe
