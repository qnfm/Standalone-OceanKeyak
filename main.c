/*
The eXtended Keccak Code Package (XKCP)
https://github.com/XKCP/XKCP

Keyak, designed by Guido Bertoni, Joan Daemen, Michaël Peeters, Gilles Van
Assche and Ronny Van Keer.

Implementation by Gilles Van Assche and Ronny Van Keer, hereby denoted as "the
implementer".

For more information, feedback or questions, please refer to the Keccak Team
website: https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/
#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#ifndef SODIUM_STATIC
#define SODIUM_STATIC
#endif

#ifdef _WIN32
#define xstat _stat64
#else
#define xstat stat

#endif

#define ADsize 20
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "encrypt.h"
#include "sodium.h"

int main(int argc, char **argv) {
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
  memset(ptstr, 0, 200);
  memset(ctstr, 0, 200);
  memset(kstr, 0, 200);
  memset(nonce, 0, 150);

  FILE *output_file, *input_file, *key_file;
  int err;

  unsigned char *plaintext;
  unsigned char *ciphertext;
  unsigned char *key;

  int retcode;

  if (sodium_init() < 0) {
    perror("Failed to init libsodium\n");
    exit(EXIT_FAILURE);
  }

  struct xstat sbk;
  struct xstat sbpt;
  struct xstat sbct;

  // encryption mode args:{plaintext filename}
  if (argc == 2) {
    strcpy(ptstr, argv[1]);
    strcpy(ctstr, argv[1]);
    strcat(ctstr, ".Keyak");
    strcpy(kstr, ptstr);
    strcat(kstr, ".key");

#if defined _WIN32

    err = fopen_s(&key_file, kstr, "wb");
    if (err != 0) perror("The key file was not opened");

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

#endif

    if (xstat(kstr, &sbk) == -1 || xstat(ptstr, &sbpt) == -1 ||
        xstat(ctstr, &sbct) == -1) {
      perror("stat");
      exit(EXIT_FAILURE);
    }

    if (sbk.st_size != 0 || sbpt.st_size == 0 || sbct.st_size != 0) {
      perror("The key/ciphertext is not empty or the plaintext file is empty");
      exit(EXIT_FAILURE);
    }

    plaintext = malloc(sbpt.st_size);
    ciphertext = malloc(sbpt.st_size + tagsize);
    // keyfile is: key||nonce
    key = malloc(keysize + noncesize);

    randombytes_buf(key, keysize);
    fwrite(key, sizeof(unsigned char), keysize + noncesize, key_file);

    memcpy(nonce, key + keysize, 16);

#if defined _WIN32
    fread_s(plaintext, sbpt.st_size, sbpt.st_size, sizeof(unsigned char),
            input_file);
#else
    fread(plaintext, sizeof(unsigned char), sbpt.st_size, input_file);
#endif

    unsigned long long clen;
    retcode = crypto_aead_encrypt(ciphertext, &clen, plaintext, sbpt.st_size,
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
#endif

    if (stat(kstr, &sbk) == -1 || stat(ptstr, &sbpt) == -1 ||
        stat(ctstr, &sbct) == -1) {
      perror("stat");
      exit(EXIT_FAILURE);
    }

    if (sbk.st_size == 0 || sbct.st_size == 0 || sbpt.st_size != 0) {
      perror("The key/ciphertext is empty or the plaintext file is not empty");
      exit(EXIT_FAILURE);
    }

    plaintext = malloc(sbct.st_size - tagsize);
    ciphertext = malloc(sbct.st_size);
    // keyfile is: key||nonce
    key = malloc(keysize + noncesize);

#if defined _WIN32
    fread_s(key, sbk.st_size, sizeof(unsigned char), sbk.st_size, key_file);
    fread_s(ciphertext, sbct.st_size, sizeof(unsigned char), sbct.st_size,
            input_file);
#else
    fread(key, sizeof(unsigned char), sbk.st_size, key_file);
    fread(ciphertext, sizeof(unsigned char), sbct.st_size, input_file);
#endif

    memcpy(nonce, key + keysize, 16);

    unsigned long long mlen;
    retcode = crypto_aead_decrypt(plaintext, &mlen, 0, ciphertext, sbct.st_size,
                                  AD, ADsize, nonce, key);
    if (retcode != 0) {
      perror("!!! crypto_aead_decrypt() did not return 0");
      exit(EXIT_FAILURE);
    }

    if (mlen != sbct.st_size - tagsize) {
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
