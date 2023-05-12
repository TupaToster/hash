#include "protos.h"

hash_t countHashStr (char* val) {

    return countHash (val, val + strlen (val));
}

int main () {

    HashTable lol (countHashStr);
    lol.insert ("aye", 11);
    // dump (lol);
}