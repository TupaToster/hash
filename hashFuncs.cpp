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

        retVal += *str;
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
        retVal = rol (retVal) + *str;

    return retVal;
}

hash_t rorHash (char* str) {

    assert (str != NULL);

    hash_t retVal = 0;

    for (;*str != '\0';str++)
        retVal = ror (retVal) + *str;

    return retVal;
}