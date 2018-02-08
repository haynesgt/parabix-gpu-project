#include <stdio.h>
#include <stdlib.h>

#ifndef BUFFERSIZE
#define BUFFERSIZE 4096
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

void cat(FILE *f);

int main(
    int argc,
    char *argv[]
) {
    if (argc == 0) {
        cat(stdin);
    } else {
        int i;
        for (i = 1; i < argc; i++) {
            FILE *f;
            f = fopen(argv[i], "r");
            if (f == NULL) {
                perror("fopen");
            }
            cat(f);
            if (EOF == fclose(f)) {
                perror("fopen");
            }
        }
    }
    return EXIT_SUCCESS;
}

void cat(FILE *f) {
    char buffer[BUFFERSIZE];
    size_t nread = 0;
    size_t total = 0;
    while (BUFFERSIZE == (
        nread = fread(buffer, 1, BUFFERSIZE, f)),
        total += nread,
        nread
    );
    if (ferror(f)) {
        perror("fread");
    }
    if (DEBUG) {
        fprintf(stderr, "%ld\n", total);
    }
}
