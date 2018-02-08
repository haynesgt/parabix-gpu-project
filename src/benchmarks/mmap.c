#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef BUFFERSIZE
#define BUFFERSIZE 4096
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

void cat(int fd);

int main(
        int argc,
        char *argv[]
        ) {
    if (argc == 0) {
        cat(STDIN_FILENO);
    } else {
        int i;
        for (i = 1; i < argc; i++) {
            int fd;
            fd = open(argv[i], O_RDONLY);
            if (fd == -1) {
                perror("open");
            }
            cat(fd);
            if (EOF == close(fd)) {
                perror("close");
            }
        }
    }
    return EXIT_SUCCESS;
}

void cat(int fd) {
    int off = 0;
    char buffer[BUFFERSIZE];

    struct stat statinfo;
    memset(&statinfo, 0, sizeof(statinfo));
    fstat(fd, &statinfo);

    int file_size = statinfo.st_size;

    size_t total = 0;

    size_t read_size = BUFFERSIZE;

    while (off < file_size)
    {
        void* data;
        if (off + read_size > file_size) {
            read_size = file_size - off;
        }
        data = mmap(NULL, read_size, PROT_READ, MAP_PRIVATE, fd, off);
        if (data == NULL) {
            perror("mmap");
        }

        total += read_size;

        munmap(data, read_size);
        off += BUFFERSIZE;
    }
    if (DEBUG) {
        fprintf(stderr, "%ld\n", total);
    }
}
