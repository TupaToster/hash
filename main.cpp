#include "protos.h"

hash_t countHashStr (char* val) {

    return countHash (val, val + strlen (val));
}

int main () {

    HashTable lol (countHashStr);
    lol.insert ("aye", 11);
    lol.insert ("suka", -1);
    lol.insert ("pizdec", 228);
    lol.insert ("420", 240);
    dump (lol);
    lol.del ("suka");
    dump (lol);
}