#ifndef EXTERN_H
#define EXTERN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <err.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

#include "readbytes.h"
#include "readlines.h"
#define NO_ARG "No arguments!"


/*error open file
#define ERR_OPEN "Failed to open file."
#define ERR_NO_FILE "File does not exist."
#define ERR_MANY_FILES "Too many open files."
#define ERR_ACCES "Denial of access."
#define ERR_INVACC "Invalid access code."
*/
#define LENGTH_BUFFER 1024
#define number_row 10
#define SIZE_T_MAX (~(size_t)0)
#define EOF (-1)
#define readchar(fd, b, n){     \
    int e;                      \
    e = read(fd,b,n);           \
    if(!(e>=0)){                \
        errno=EXIT_FAILURE;     \
        perror("Read error");   \
        return;                 \
    }                           \
}

#define writechar(b,n){         \
    int e;                      \
    e = write(1,b,n);           \
    if(e<0){                    \
        errno=EXIT_FAILURE;     \
        perror("Write error");  \
        return;                 \
    }                           \
}
void print_tail(int fd);

struct globalArgs_t {
    size_t N;                  /*number N*/
    int numInputFiles;      /*number of input files*/
    int name;              /*quiet | verbose*/
    char **inputFiles;      /*name input files*/
    int style;
} globalArgs;


struct stat sb;


char ch;
char *buf[LENGTH_BUFFER];
int len_buf;
char* fname;
#endif
