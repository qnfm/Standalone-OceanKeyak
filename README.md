# Standalone-OceanKeyak
an AE scheme extracted from XKCP(eXtended Keccak Code Package)
* [`Keyak`](https://github.com/XKCP/XKCP/blob/master/doc/Keyak-documentation.h), the authenticated encryption schemes River, Lake, Sea, Ocean and Lunar Keyak.This repo only extracts the OceanKeyak 

# Build

Clone the code and change into the source code directory

$ git clone https://github.com/qnfm/Standalone-OceanKeyak && cd Standalone-OceanKeyak

If you have downloaded the ninja_build binary, the build command is:

$ cmake "-DCMAKE_MAKE_PROGRAM=C:/path_to_ninja/ninja.exe" -G Ninja -S . -B cmake-build-debug

Or run build without ninja:

$ cmake -S . -B cmake-build-debug

Final step: link the program:

$ cmake --build cmake-build-debug --target Keyak -j 3

The binary is located in the x64(x84)/Debug directory

# Under which license is the XKCP distributed?

Most of the source and header files in the XKCP are released to the **public domain** and associated to the [CC0](http://creativecommons.org/publicdomain/zero/1.0/) deed. The exceptions are the following:

* [`brg_endian.h`](lib/common/brg_endian.h) is copyrighted by Brian Gladman and comes with a BSD 3-clause license;
