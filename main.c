#include "extern.h"
#include "readbytes.h"

static const char *optString ="b:c:n:qv";
static const char *usage = "usage: tail [-n # | -c #] [-q | -v] [file ...]\n";
static const char *missedname ="Missing namefile";

#define get_arg(units,type){                        \
    if (globalArgs.style != 0){                     \
        perror(usage);                              \
        exit(1);                                    \
    }                                               \
    globalArgs.N = strtol(optarg, &e, 10)* (units); \
    if(*e)                                          \
        err(1,"illegal offset -- %s",optarg);       \
    switch(optarg[0]) {                             \
        case '+':                                   \
            globalArgs.style = (type);              \
            break;                                  \
        case '-':                                   \
            globalArgs.N=-globalArgs.N;             \
        default:                                    \
            globalArgs.style = -(type);             \
            break;                                  \
    }                                               \
}


int main(int argc, char** argv)
{
    /*Take argument*/
    /*int file_des;*/
    int opt;
    char* e;
    int fd;
    fname = argv[1];
    
/*Create empty buffer*/

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
        for(;(fname = *argv++);){
            if(strlen(fname)==1 && fname[0]=='-'){
                if(argc>1){
                    errno=E2BIG;
                    perror("Error");
                    exit(errno);
                }
                fd=0;
                print_tail(fd);
                write(1,"\n",1);
            } else {
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
                write(1,"\n",1);
                close(fd);
            }
        }
    } else {
        errno=EXIT_FAILURE;
        perror(missedname);
        exit(errno);
    }
    return 0;
}


void print_tail(int fd){
    if (globalArgs.N==0)
            return;
	size_t NN = globalArgs.N;
    switch(globalArgs.style) {
    case 'b':
        /*Check that the file is regular*/
        if (fd != 0 && S_ISREG(sb.st_mode)) {
            /*if the file size is less than N, then print a fully.*/
            if ((unsigned int)sb.st_size < NN)
                NN = sb.st_size;
            if (lseek(fd,0,SEEK_SET)==-1){
                errno=EXIT_FAILURE;
                perror("Read error");
                return;
            }
        } else {
            fbytes(NN);
            return;
        }
        break;
    case (-'b'):
        /*Check that the file is regular*/
        if (S_ISREG(sb.st_mode)) {
            /*if the file size is less than N, then print a fully.*/
            if ((unsigned int)sb.st_size < NN)
                NN = sb.st_size;

            if (lseek(fd,-NN,SEEK_END)==-1){
                errno=EXIT_FAILURE;
                perror("Read error");
                return;
            }
        } else {
            bytes(NN);
            return;
        }
        break;
    case 'l':
        if (fd != 0 && S_ISREG(sb.st_mode)) {
        int newN=0;
        for (;;){
            readchar(fd,&ch,1);
            if (ch==EOF)
                break;
             else {
                newN++;
                if(ch == '\n' && !--NN){
                    break;
                }
            }
        }
        NN=newN;
       
        if (lseek(fd,0,SEEK_SET)==-1){
                errno=EXIT_FAILURE;
                perror("Read error");
                return;
       } } else{
		flines(NN);
		return;
		}
        break;
    case (-'l'):
        /*Check that the file is regular*/
        if (fd != 0 && S_ISREG(sb.st_mode)) {
            register off_t size = sb.st_size;
            register char *p;
            char *start;
        /*Check file size*/
            if((unsigned long) size > SIZE_T_MAX){
                err(0,"%s: %s",fname, strerror(EFBIG));
                return;
            }
            if ((start = mmap(NULL,(size_t)size,PROT_READ,
		MAP_PRIVATE,fd,(off_t)0)) == MAP_FAILED){
                 err(0,"%s: %s",fname, strerror(errno));
                return;
            }
            for (p=start+size-1;size--;){
                if(*--p == '\n' && !--NN){
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
        } else {
		lines(NN);	
			return;
	}

        break;
    }
    /*Print byte*/
        while(NN>0){
            int N = LENGTH_BUFFER;

            if(NN < LENGTH_BUFFER){
                N = NN;
            }
            readchar(fd,buf,N);
            writechar(buf,N)
            NN-=N;
        }
}
