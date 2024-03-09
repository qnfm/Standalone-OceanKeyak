#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include "UT.h"

void UT_startTest(const char *synopsis, const char *implementation)
{
#if !defined(UT_EMBEDDED)
    if (strlen(implementation) == 0)
        printf("* %s\n", synopsis);
    else
        printf("* %s: %s\n", synopsis, implementation);
    fflush(stdout);
#endif
}

void UT_endTest()
{
#if !defined(UT_EMBEDDED)
    printf("    - OK\n");
    fflush(stdout);
#endif
}

void UT_displayInfo(const char *header, const char *contents)
{
#if !defined(UT_EMBEDDED)
    printf("  %s: %s\n", header, contents);
    fflush(stdout);
#endif
}

#ifndef UT_EMBEDDED
void UT_displayByteString(FILE *f, const char* synopsis, const unsigned char *data, unsigned int length)
{
    unsigned int i;

    fprintf(f, "%s:", synopsis);
    for(i=0; i<length; i++)
        fprintf(f, " %02x", (unsigned int)data[i]);
    fprintf(f, "\n");
}
#endif

void UT_assert(int condition, char * synopsis)
{
    if (!condition)
    {
        #ifdef UT_EMBEDDED
        for ( ; ; ) ;
        #else
        printf("%s\n", synopsis);
        exit(1);
        #endif
    }
}

void UT_generateSimpleRawMaterial(unsigned char* data, unsigned int length, unsigned char seed1, unsigned int seed2)
{
    unsigned int i;

    for(i=0; i<length; i++) {
        unsigned char iRolled = ((unsigned char)i << seed2) | ((unsigned char)i >> (8-seed2));
        unsigned char byte = seed1 + 161 * (length - iRolled + i);
        data[i] = byte;
    }
}

#ifdef __cplusplus
}
#endif