#include <immintrin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashFuncs.h"

int main () {

    HashTable lol (rorHash, 7);
    for (int i = 0; i < 10000; i++) {
        lol.insert ("aye", 11);
        lol.insert ("suka", -1);
        lol.insert ("pizdec", 228);
        lol.insert ("420", 240);
        lol.del ("suka");
    }
    dump (lol);
}