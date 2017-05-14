#include <stdio.h>
#include <stdlib.h>

#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <unistd.h>
#include <errno.h>
#include "err_message.h"


#define LENGTH_BUFFER 1024
#define number_row 10
#define SIZE_T_MAX (~(size_t)0)

#define get_arg(units,type){                             \
    if (globalArgs.style != 0){                                     \
        perror(usage);                              \
        exit(1);                                    \
    }                                               \
    globalArgs.N = strtol(optarg, &e, 10)* (units); \
    if(*e)                                          \
        err(1,"illegal offset -- %s",optarg);       \
    switch(optarg[0]) {                             \
        case '+':                                   \
            globalArgs.style = (type);                  \
            break;                                  \
        case '-':                                   \
            globalArgs.N=-globalArgs.N;             \
        default:                                    \
            globalArgs.style = -(type);                     \
            break;                                  \
    }                                               \
}


int num = number_row;

struct globalArgs_t {
    long N;                  /*number N*/
    int numInputFiles;      /*number of input files*/
    int name;              /*quiet | verbose*/
    char **inputFiles;      /*name input files*/
    int style;
} globalArgs;

struct stat sb;


static const char *optString ="b:c:n:qv";
static const char *usage = "usage: tail [-n # | -c #] [-q | -v] [file ...]\n";
static const char *missedname ="Missing namefile";


char *buf[LENGTH_BUFFER];
char *ch[1];
int len_buf;
char* fname;



int main(int argc, char** argv)
{
    /*Take argument*/
    fname = argv[1];
    int file_des;
    int opt;
    char* e;
    int fd;

    /*Create empty buffer*/
    char buf[LENGTH_BUFFER] = {0};
    int len = -1;

    if (!fname) {
        perror(NO_ARG);
        exit(EXIT_FAILURE);
    }

    /*globalArgs initialization*/
    globalArgs.N=0;
    globalArgs.inputFiles=NULL;
    globalArgs.numInputFiles=0;
    globalArgs.name=0;
    globalArgs.style=0;


    while((opt = getopt(argc,argv,optString)) != -1){
        switch (opt){
            case 'c':
                get_arg(1,'b');
                break;
            case 'v':
                if(globalArgs.name == 0)
                    globalArgs.name='v';
                else{
                    errno=EINVAL;
                    perror(usage);
                    exit(errno);
                }
                break;
            case 'q':
                if(globalArgs.name == 0)
                    globalArgs.name='q';
                else{
                    errno=EINVAL;
                    perror(usage);
                    exit(errno);
                }
                break;
            case 'n':
                get_arg(1,'l');
                break;
            case 'b':
                get_arg(512,'c');
                break;
            case '?':
                errno=EINVAL;
                perror(usage);
                exit(errno);
        }
    }
    argc-=optind;
    argv +=optind;
    if(globalArgs.style==0){
        globalArgs.style=-'l';
        globalArgs.N=10;
    }


    if(*argv){
        for(;fname = *argv++;){
            int fd = open(fname, O_RDONLY);
            if (fd == -1 || fstat(fd,&sb)){
                perror(fname);
                continue;
            }
            if((argc > 1 && globalArgs.name != 'q') || (globalArgs.name == 'v')){
                write(1,"\n==>",4);
                write(1,fname,strlen(fname));
                write(1,"<==\n",4);
            }
            print_tail(fd);
            close(fd);
        }
    } else {
        errno=EXIT_FAILURE;
        perror(missedname);
        exit(errno);
    }
  //  printf("type: %d\nN: %d\nNumfile: %d\nname: %d\n",globalArgs.type,
    //globalArgs.N, globalArgs.numInputFiles,globalArgs.name);



    //file_des = file_open(file_name);

    return 0;
}

#define readchar(fd, b, n){     \
    int e;                      \
    e = read(fd,b,n);           \
    if(!(e>0)){                 \
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

void print_tail(int fd){
    if (globalArgs.N==0)
            return;
    switch(globalArgs.style) {
    case 'b':
        /*Check that the file is regular*/
        if (S_ISREG(sb.st_mode)) {
            /*if the file size is less than N, then print a fully.*/
            if (sb.st_size < globalArgs.N)
                globalArgs.N = sb.st_size;

            if (lseek(fd,0,SEEK_SET)==-1){
                errno=EXIT_FAILURE;
                perror("Read error");
                return;
            }
        }
        break;
    case (-'b'):
        /*Check that the file is regular*/
        if (S_ISREG(sb.st_mode)) {
            /*if the file size is less than N, then print a fully.*/
            if (sb.st_size < globalArgs.N)
                globalArgs.N = sb.st_size;

            if (lseek(fd,-globalArgs.N,SEEK_END)==-1){
                errno=EXIT_FAILURE;
                perror("Read error");
                return;
            }
        }
        break;
    case 'l':
        {
        int newN=0;
        for (;;){
            readchar(fd,ch,1);
            if (ch[0]==EOF)
                break;
             else {
                newN++;
                if(ch[0] == '\n' && !--globalArgs.N){
                    break;
                }
            }
        }
        globalArgs.N=newN;
        }

        if (lseek(fd,0,SEEK_SET)==-1){
                errno=EXIT_FAILURE;
                perror("Read error");
                return;
        }
        break;
    case (-'l'):


        /*Check that the file is regular*/
        if (S_ISREG(sb.st_mode)) {
            register off_t size = sb.st_size;
            register char *p;
            void *start;
        /*Check file size*/
        fflush(stdout);
            if(size > SIZE_T_MAX){
                err(0,"%s: %s",fname, strerror(EFBIG));
                return;
            }
            if ((start = mmap(NULL,(size_t)size,PROT_READ,MAP_PRIVATE,fd,(off_t)0)) == MAP_FAILED){
                 err(0,"%s: %s",fname, strerror(errno));
                return;
            }
            for (p=start+size-1;size--;){
                if(*--p == '\n' && !--globalArgs.N){
                    ++p;
                    break;
                }
                if(size-1==0)
                    break;
                }
            size =  sb.st_size -size;
            if(write(1,p,size) !=size)
                perror("Write error");
            if (munmap(start, (size_t)sb.st_size)) {
                            err(0, "%s: %s", fname, strerror(errno));
                return;
            }
            return;

        }

       printf("-l");

        break;
    }
    /*Print byte*/
   // if(globalArgs.style == 'b' ||
   // globalArgs.style == -'b'){
        while(globalArgs.N>0){
            int N = LENGTH_BUFFER;

            if(globalArgs.N < LENGTH_BUFFER){
                N = globalArgs.N;
            }
            readchar(fd,buf,N);
            writechar(buf,N)
            globalArgs.N-=N;
        }
   // }

}