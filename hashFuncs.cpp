#include "hashFuncs.h"

hash_t one (char* str) {

    return 1;
}

hash_t firstAscii (char* str) {

    assert (str != NULL);

    return *str;
}

hash_t lenHash (char* str) {

    assert (str != NULL);

    return strlen (str);
}

hash_t asciiSum (char* str) {

    assert (str != NULL);

    hash_t retVal = 0;

    while (*str != '\0') {

        retVal += (unsigned char) *str;
        str++;
    }

    return retVal;
}

hash_t ror (hash_t val) {

    hash_t bit = val & 1;
    val >>= 1;
    val |= (bit << (8 * sizeof (val) - 1));
    return val;
}

hash_t rol (hash_t val) {

    hash_t bit = val & (1 << (8 * sizeof (val) - 1));
    bit >>= (sizeof (val) * 8 - 1);
    val <<= 1;
    val |= bit;
    return val;
}

hash_t rolHash (char* str) {

    assert (str != NULL);

    hash_t retVal = 0;

    for (;*str != '\0'; str++)
        retVal = rol (retVal) + (unsigned char)*str;

    return retVal;
}

hash_t rorHash (char* str) {

    assert (str != NULL);

    hash_t retVal = 0;

    for (;*str != '\0';str++)
        retVal = ror (retVal) + *str;

    return retVal;
}

hash_t crc32Tab[256] = {0};
bool filled = false;
const hash_t crc32Poly = 0xEDB88320;

void fillCrc32 () {

    filled = true;

    for (int i = 0; i < 256; i++) {

        crc32Tab[i] = i;
        for (int j = 0; j < 8; j++)
            crc32Tab[i] = (crc32Tab[i] & 1) ? (crc32Tab[i] >> 1) ^ crc32Poly : (crc32Tab[i] >> 1);

    }
}

hash_t crc32Hash (char* str) {

    assert (str != NULL);

    if (!filled) fillCrc32 ();

    hash_t retVal = -1;

    while (*str != '\0')
        retVal = crc32Tab[(unsigned char) retVal ^ (*(str++))] ^ (retVal >> 8);

    return ~retVal;
}