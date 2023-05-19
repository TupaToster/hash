#include <immintrin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashFuncs.h"

hash_t countHashStr (char* val) {

    return countHash (val, val + strlen (val));
}

int main () {

    
}