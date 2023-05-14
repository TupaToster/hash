#include "protos.h"

hash_t countHashStr (char* val) {

    return countHash (val, val + strlen (val));
}

int main () {

    HashTable lol (countHashStr);
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < 100; j++) countHashStr ("lol");
        lol.insert ("aye", 11);
        lol.insert ("suka", -1);
        lol.insert ("pizdec", 228);
        lol.insert ("420", 240);
        lol.del ("suka");
    }
    dump (lol);
    lol.save2file ("out.lol");
}