#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS

#include <io.h>
#include <fcntl.h>

#else

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#endif


#define ADsize 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "encrypt.h"
#include <sodium.h>

int main(int argc, char **argv) {

    if (sodium_init() < 0) {
        perror("Failed to init libsodium\n");
        exit(EXIT_FAILURE);
    }

    const size_t keysize = 16, noncesize = 16, tagsize = 16;
    // domain seperator: "StandaloneOceanKeyak"
    // const unsigned char* AD = (const unsigned char*)"\x32\xf3\xb4\x75\x35\xf6";
    const unsigned char AD[ADsize] = {0x53, 0x74, 0x61, 0x6e, 0x64, 0x61, 0x6c,
                                      0x6f, 0x6e, 0x65, 0x4f, 0x63, 0x65, 0x61,
                                      0x6e, 0x4b, 0x65, 0x79, 0x61, 0x6b};
    char ptstr[200];
    char ctstr[200];
    char kstr[200];
    unsigned char nonce[150];

    sodium_memzero(ptstr, 200);
    sodium_memzero(ctstr, 200);
    sodium_memzero(kstr, 200);
    sodium_memzero(nonce, 150);

    FILE *output_file, *input_file, *key_file;
    int err;

    unsigned char *plaintext;
    unsigned char *ciphertext;
    unsigned char *key;

    int retcode;

    // encryption mode args:{plaintext filename}
    if (argc == 2) {
        strcpy(ptstr, argv[1]);
        strcpy(ctstr, argv[1]);
        strcat(ctstr, ".Keyak");
        strcpy(kstr, ptstr);
        strcat(kstr, ".key");

#if defined _WIN32

        int fd;
        _sopen_s(&fd, "foo.bin", _O_RDONLY, _SH_DENYRW, _S_IREAD);

        err = fopen_s(&key_file, kstr, "wb");
        if (err != 0) perror("The key file was not opened");

        err = _sopen_s(&fd, ptstr, _O_RDONLY, _SH_DENYRW, _S_IREAD);
        if (err != 0) perror("The input file was not opened");
        __int64 ptsz = _filelengthi64(fd);
        err = _close(fd);
        if (err != 0) perror("Failed to closed the input file");

        err = fopen_s(&input_file, ptstr, "rb");
        if (err != 0) perror("The input file was not opened");

        err = fopen_s(&output_file, ctstr, "wb");
        if (err != 0) perror("The output was not opened");


#else

        key_file = fopen(kstr, "wb");
        if (!key_file) perror("The key file was not opened");

        input_file = fopen(ptstr, "rb");
        if (!input_file) perror("The input file was not opened");

        output_file = fopen(ctstr, "wb");
        if (!output_file) perror("The output was not opened");
        if (!key_file || !input_file || !output_file) return -1;

        off_t ptsz = ftello(input_file);

#endif

        plaintext = malloc(ptsz);
        ciphertext = malloc(ptsz + tagsize);
        // keyfile is: key||nonce

        size_t wholekeysize = keysize + noncesize;

        key = (unsigned char *) sodium_malloc(wholekeysize);
        sodium_memzero(key, wholekeysize);
        randombytes_buf(key, wholekeysize);

        fwrite(key, sizeof(unsigned char), wholekeysize, key_file);

        memcpy(nonce, key + keysize, 16);

#if defined _WIN32
        fread_s(plaintext, ptsz, ptsz, sizeof(unsigned char),
                input_file);
#else
        fread(plaintext, sizeof(unsigned char), ptsz, input_file);
#endif

        unsigned long long clen;
        retcode = crypto_aead_encrypt(ciphertext, &clen, plaintext, ptsz,
                                      AD, ADsize, 0, nonce, key);
        if (retcode != 0) {
            perror("!!! crypto_aead_encrypt() did not return 0.\n");
            exit(EXIT_FAILURE);
        }
        fwrite(ciphertext, sizeof(unsigned char), clen, output_file);
    }
        // decryption mode args:{keyfile, ciphertext}
    else if (argc == 3) {
        strcpy(kstr, argv[1]);
        strcpy(ctstr, argv[2]);
        strncpy(ptstr, ctstr, strlen(argv[2]) + 1 - sizeof(".Keyak"));

#if defined _WIN32

        int fd;
        err = _sopen_s(&fd, ctstr, _O_RDONLY, _SH_DENYRW, _S_IREAD);
        if (err != 0) perror("The input file was not opened");
        __int64 ctsz = _filelengthi64(fd);
        err = _close(fd);
        if (err != 0) perror("Failed to closed the input file");

        err = fopen_s(&key_file, kstr, "rb");
        if (err != 0) perror("Key file was not opened");

        err = fopen_s(&input_file, ctstr, "rb");
        if (err != 0) perror("Input file was not opened");

        err = fopen_s(&output_file, ptstr, "wb");
        if (err != 0) perror("Output was not opened");
#else
        key_file = fopen(kstr, "rb");
        if (!key_file) perror("The key file was not opened");

        input_file = fopen(ctstr, "rb");
        if (!input_file) perror("The input file was not opened");

        output_file = fopen(ptstr, "wb");
        if (!output_file) perror("The output was not opened");
        if (!key_file || !input_file || !output_file) return -1;

        off_t ksz = ftello(key_file);
        off_t ctsz = ftello(input_file);

#endif

        plaintext = malloc(ctsz - tagsize);
        ciphertext = malloc(ctsz);
        // keyfile is: key||nonce
        key = malloc(keysize + noncesize);

#if defined _WIN32
        fread_s(key, 32, sizeof(unsigned char), 32, key_file);
        fread_s(ciphertext, ctsz, sizeof(unsigned char), ctsz, input_file);
#else
        fread(key, sizeof(unsigned char), ksz, key_file);
        fread(ciphertext, sizeof(unsigned char), ctsz, input_file);
#endif

        memcpy(nonce, key + keysize, 16);

        unsigned long long mlen;
        retcode = crypto_aead_decrypt(plaintext, &mlen, 0, ciphertext, ctsz,
                                      AD, ADsize, nonce, key);
        if (retcode != 0) {
            perror("!!! crypto_aead_decrypt() did not return 0");
            exit(EXIT_FAILURE);
        }

        if (mlen != ctsz - tagsize) {
            perror("!!! plaintext length mistach.");
            exit(EXIT_FAILURE);
        }
        fwrite(plaintext, sizeof(unsigned char), mlen, output_file);
    }
        // unexpected behaviour
    else {
        perror(
                "Error, usage: path_to_keyfile path_to_plaintext path_to_ciphertext");
        exit(EXIT_FAILURE);
    }

    fclose(input_file);
    fclose(output_file);
    fclose(key_file);
    free(plaintext);
    free(ciphertext);
    free(key);
    return retcode;
}
