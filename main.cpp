#include <immintrin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashFuncs.h"

int main (int argc, char* argv[]) {

    const char* callGrind = "valgrind --tool=callgrind --callgrind-out-file=";
    const char* callProg = "./hash";
    char command[150] = "";
    const int subSize = 7;
    char* names[subSize] = {"oneHash", "firstAsciiHash", "lenHash", "asciiSumHash", "rolHash", "rorHash", "crc32Hash"};

    size_t cap = 71;

    HashTable subjects[] = {HashTable (one, cap),
                            HashTable (firstAscii, cap),
                            HashTable (lenHash, cap),
                            HashTable (asciiSum, cap),
                            HashTable (rolHash, cap),
                            HashTable (rorHash, cap),
                            HashTable (rolHash, cap)};

    // Testing mode
    if (argc == 1) {

        for (int i = 0; i < subSize; i++) {

            sprintf (command, "%s%s.out.callgrind %s %d", callGrind, names[i], callProg, i);
            system (command);     // calls callgrind with arg that specifies function and output file with right name

            sprintf (command, "gnuplot -e \"fileName=\'%s.hist\'; set output \'%s.png\'\" gnuPlotScript.gp", names[i], names[i]);

            system (command);
        }

        return 0;
    }

    // mode to be tested
    assert (argc == 2);

    int* listLens = (int*) calloc (cap, sizeof (int));
    assert (listLens != NULL);

    int iter = argv[1][0] - '0';

    subjects[iter].loadFromFile ("input.lol");
    subjects[iter].saveListLens (listLens);

    sprintf (command, "%s.hist", names[iter]);

    FILE* outFile = fopen (command, "wb");
    assert (outFile != NULL);

    for (int i = 0; i < cap; i++) fprintf (outFile, "%d\n", listLens[i]);

    fclose (outFile);
    free (listLens);
}