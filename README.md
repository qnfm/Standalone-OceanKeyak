# Standalone-OceanKeyak
An cross-platform file encryption tool with AE scheme extracted from XKCP(eXtended Keccak Code Package)
* [`Keyak`](https://github.com/XKCP/XKCP/blob/master/doc/Keyak-documentation.h), the authenticated encryption schemes River, Lake, Sea, Ocean and Lunar Keyak.This repo only extracts the OceanKeyak 

# Dependencies

## CMake

Keyak requires CMake 3.10.0 or newer in order to take advantage of improved support. If you're using Ubuntu 18.04 or newer you should already have a suitable version. If you're using an older distribution (e.g. 16.04) you will need to install a compatible version of CMake yourself. Also, Keyak utilizes the libsodium api to generate unpredictable data, suitable for creating secret keys, so you should install [libsodium](https://download.libsodium.org/libsodium/releases/) into your system path if you are using linux distribution, or if you are using windows, you can type the following commad to set the sodium_DIR environment variable to point at the directory of pre-built libsodium from official website.

```cmd
set sodium_DIR path_to_pre-built_lib
```

To install libsodium on Linux:

```bash
sudo apt install libsodium-dev
```

# Build

Clone the code and change into the source code directory

```bash
git clone https://github.com/qnfm/Standalone-OceanKeyak && cd Standalone-OceanKeyak
```

If you have downloaded the ninja_build binary on Windows, the build command is:

```bash
cmake "-DCMAKE_MAKE_PROGRAM=C:/path_to_ninja/ninja.exe" -G Ninja -S . -B cmake-build-debug -Dsodium_USE_STATIC_LIBS=ON
```

Or run build without ninja:

```bash
cmake -S . -B cmake-build-debug -Dsodium_USE_STATIC_LIBS=ON
```

Final step: link the program:

```bash
cmake --build cmake-build-debug --target Keyak -j 8
```

The binary is located in the cmake-build-debug/Release|Debug directory on Windows and cmake-build-debug/ directory on linux

# Usage

To encrypt, you simply need to provide thepath to the filename, remember to keep the generated two files safely and secretly. If you lose these two file (one with .key surfix and the other with .Keyak surfix), the original file will be lost forever !

```
Keyak file_to_be_encrypted
```

To decrypt, put the two output files at the same directory and simply call:

```
Keyak path_to_key.key path_to_ciphertext.Keyak
```

# Under which license is the XKCP distributed?

Most of the source and header files in the XKCP are released to the **public domain** and associated to the [CC0](http://creativecommons.org/publicdomain/zero/1.0/) deed. The exceptions are the following:

* [`brg_endian.h`](lib/common/brg_endian.h) is copyrighted by Brian Gladman and comes with a BSD 3-clause license;
