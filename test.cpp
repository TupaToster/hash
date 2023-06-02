#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <assert.h>

int main () {

    const int testCnt = 100;
    volatile double avgTime = 0;

    for (volatile int i = 0; i < testCnt; i++)
        system ("./hash 4");

    FILE* timeFile = fopen ("times.txt", "rb");
    assert (timeFile != NULL);

    for (int i = 0; i < testCnt; i++) {

        size_t time = 0;
        fscanf (timeFile, " %llu", &time);
        avgTime += (double) time;
    }

    avgTime /= (double) testCnt;

    printf ("Average work time in cpu ticks : %.2lf\n", avgTime);

    fclose (timeFile);

    remove ("times.txt");

    system ("valgrind --tool=callgrind ./hash 4");
}