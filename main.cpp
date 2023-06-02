#include <immintrin.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hashFuncs.h"
#include "rngStrArray.h"

#define op1 1

#ifdef op1
#define rolHash _rolHash
#endif


extern "C" unsigned int _rolHash (char*);

int main (int argc, char* argv[]) {

    const char* callGrind = "valgrind --tool=callgrind --callgrind-out-file="; // command to call callgrind
    const char* callProg = "./hash"; // base prog call command
    const char* timeFileName = "times.txt";
    char inputFile[] = "input.lol"; // hash table input file
    char command[150] = ""; //command buffer
    const int subSize = 7; // total amount of subjects for research
    char* names[subSize] = {"oneHash", "firstAsciiHash", "lenHash", "asciiSumHash", "rolHash", "rorHash", "crc32Hash"}; // names of functions used for each subject

    // For find part random string are generated before each run. I did it this way because there is no need to actually search for strings present in the table, we just need some hash and a string to strcmp to
    const int rngStrLen = 32; // length of random string
    const int rngStrCnt = 1000000; // amount of random strings aka amount of searches

    const size_t cap = 1479; // calculated as 7710 / 30 which is amount of strings divided by desired list len with perfect distribution

    // Testing mode
    if (argc == 1) {

        for (int i = subSize - 1; i >= 0; i--) {

            sprintf (command, "%s%s.out.callgrind %s %d", callGrind, names[i], callProg, i);
            system (command);     // calls callgrind with arg that specifies function and output file with right name

            sprintf (command, "gnuplot -e \"fileName=\'%s.hist\'; set output \'%s.png\'\" gnuPlotScript.gp", names[i], names[i]);

            system (command);
        }

        return 0;
    }

    // mode to be tested
    assert (argc == 2);

    HashTable subjects[] = {HashTable (one, cap),
                            HashTable (firstAscii, cap),
                            HashTable (lenHash, cap),
                            HashTable (asciiSum, cap),
                            HashTable (rolHash, cap),
                            HashTable (rorHash, cap),
                            HashTable (crc32Hash, cap)};

    int* listLens = (int*) calloc (cap, sizeof (int));
    assert (listLens != NULL);

    int iter = argv[1][0] - '0';

    subjects[iter].loadFromFile (inputFile);
    subjects[iter].saveListLens (listLens);

    sprintf (command, "%s.hist", names[iter]);
    FILE* outFile = fopen (command, "wb");
    assert (outFile != NULL);

    for (int i = 0; i < cap; i++) fprintf (outFile, "%d\n", listLens[i]);

    FILE* timeFile = fopen (timeFileName, "ab");
    assert (timeFile != NULL);

    size_t time = _rdtsc ();

    for (volatile int i = 0; i < rngStrCnt; i++)
        subjects[iter].find ((char*) rngTestSet[i]);

    time = _rdtsc () - time;

    fprintf (timeFile, "%llu\n", time);

    fclose (timeFile);

    fclose (outFile);
    free (listLens);

    for (int i = 0; i < subSize; i++) subjects[i].DTOR();
}