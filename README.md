RafOS
=====

General
-------
RafOS is simple DOS-like operating system made by me in school years as hobby project.

Building - Windows
------------------
Install MinGW.
Install ImDisk from: http://www.ltr-data.se/opencode.html/

Building - Linux
----------------

Generate Makefiles:

	tools/linux/premake5 gmake

Compile project:

	CC=i686-w64-mingw32-gcc CXX=i686-w64-mingw32-g++ make -C build

TODO
