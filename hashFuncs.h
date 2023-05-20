#pragma once
#include "hash.h"

hash_t one (char* str);

hash_t firstAscii (char* str);

hash_t lenHash (char* str);

hash_t asciiSum (char* str);

hash_t rol (hash_t val);

hash_t ror (hash_t val);

hash_t rolHash (char* str);

hash_t rorHash (char* str);

extern hash_t crc32Tab[256];

hash_t crc32Hash (char* str);