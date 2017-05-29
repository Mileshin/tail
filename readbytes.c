#include "extern.h"
void bytes(off_t N){
    char *sp;
    register char *p, *ep;
    register int len;
    int w;
    if ((sp = p = malloc(N) ) == NULL)
        err(1,"%s",strerror(errno));
    if(read(STDIN_FILENO,&ch,1)<=0)
        return;
    for(w = 0, ep = p + N;ch!=EOF;){
        *p = ch;
        if(++p == ep){
            w = 1;
            p = sp;
        }
        if(read(STDIN_FILENO,&ch,1) <=0)
            break;
    }
	if((globalArgs.name == 'v'))      
     		write(1,"\n==>stdin<==\n",13);
	else
   		write(STDOUT_FILENO,"\n",1);
    if(w && (len = ep -p))
        if(write(STDOUT_FILENO, p,len) != len)
            err(1,"stdout: %s",strerror(errno));
    if((len=p-sp))
        if(write(STDOUT_FILENO, sp,len) != len)
            err(1,"stdout: %s",strerror(errno));
}

void fbytes(off_t N){
    char *sp;
    register char *p, *ep;
    register int len;
    int w=0;
    if ((sp = p = malloc(N) ) == NULL)
        err(1,"%s",strerror(errno));
    if(read(STDIN_FILENO,&ch,1)<=0)
        return;
    for(w = 0, ep = p + N;ch!=EOF;){
        if(w<N){
            *p = ch,p++;
        } 
        if(read(STDIN_FILENO,&ch,1)<=0)
		break;
        w++;
    }
	if((globalArgs.name == 'v'))      
		write(1,"\n==>stdin<==\n",13);
	else
	write(STDOUT_FILENO,"\n",1); 
    if(w && (len = ep -p+1))
        if(write(STDOUT_FILENO, p,len) != len)
            err(1,"stdout: %s",strerror(errno));
    if((len=p-sp+1))
        if(write(STDOUT_FILENO, sp,len) != len)
            err(1,"stdout: %s",strerror(errno));
	return;
}
