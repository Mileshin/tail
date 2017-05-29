#include "extern.h"
#include "readlines.h"
/*display the last offset lines of stdin*/
void lines(off_t N){
	struct {
		unsigned int blen;
		unsigned int len;
		char *l;
	} *lines;
	register char *p;
	unsigned int blen, cnt, recno, w;
	char *sp;
	if ((lines = malloc(N * sizeof(*lines))) == NULL)
		err(1, "%s", strerror(errno));

	sp = NULL;
	blen = cnt = recno = w = 0;
	
	if(read(STDIN_FILENO,&ch,1)<=0)
		return;
	while (ch != EOF) {
		if (++cnt > blen) {
			if ((sp = realloc(sp, blen += 1024)) == NULL)
				err(1, "%s", strerror(errno));
			p = sp + cnt - 1;
		}
		*p++ = ch;
		if (ch == '\n') {
			if (lines[recno].blen < cnt) {
				lines[recno].blen = cnt + 256;
				if ((lines[recno].l = realloc(lines[recno].l,
				    lines[recno].blen)) == NULL)
					err(1, "%s", strerror(errno));
			}
			bcopy(sp, lines[recno].l, lines[recno].len = cnt);
			cnt = 0,p = sp;
			if (++recno == N) 
				w = 1,recno = 0;
		}
		if(read(STDIN_FILENO,&ch,1)<=0)
			break;
	}
	if (cnt) {
		lines[recno].l = sp, lines[recno].len = cnt;
		if (++recno == N) {
			w = 1,recno = 0;
		}
	}
	
	if((globalArgs.name == 'v'))      
     		write(1,"\n==>stdin<==\n",13);
	else
   		write(STDOUT_FILENO,"\n",1);
	for (cnt = recno; (w && cnt < N); ++cnt)
		if(write(STDOUT_FILENO,lines[cnt].l, 
		  lines[cnt].len)!= lines[cnt].len)
			err(1,"stdout: %s",strerror(errno));
	for (cnt = 0; cnt < recno; ++cnt)
		if(write(STDOUT_FILENO,lines[cnt].l, 
		  lines[cnt].len)!= lines[cnt].len)
			err(1,"stdout: %s",strerror(errno));
}






/*display the forvard offset lines of stdin*/
void flines(off_t N){
	struct {
		unsigned int blen;
		unsigned int len;
		char *l;
	} *lines;
	register char *p;
	unsigned int blen, cnt, recno, w;
	char *sp;
	off_t NN=N;
	if ((lines = malloc(N * sizeof(*lines))) == NULL)
		err(1, "%s", strerror(errno));

	sp = NULL;
	blen = cnt = recno = w = 0;
	
	if(read(STDIN_FILENO,&ch,1)<=0)
		return;
	while (ch != EOF) {
		if(NN>0){
		if (++cnt > blen) {
			if ((sp = realloc(sp, blen += 1024)) == NULL)
				err(1, "%s", strerror(errno));
			p = sp + cnt - 1;
		}
		*p++ = ch;
		if (ch == '\n' && --NN>0) {
			if (lines[recno].blen < cnt) {
				lines[recno].blen = cnt + 256;
				if ((lines[recno].l = realloc(lines[recno].l,
				    lines[recno].blen)) == NULL)
					err(1, "%s", strerror(errno));
			}
			bcopy(sp, lines[recno].l, lines[recno].len = cnt);
			cnt = 0,p = sp;
			if (++recno == N) {
				w = 1,recno = 0;
			}
		}
		}
		if(read(STDIN_FILENO,&ch,1)<=0)
			break;
	}
	if (cnt) {
		lines[recno].l = sp,lines[recno].len = cnt;
		if (++recno == N) 
			w = 1,recno = 0;
	}
	
	if((globalArgs.name == 'v'))      
     		write(1,"\n==>stdin<==\n",13);
	else
   		write(STDOUT_FILENO,"\n",1);
	for (cnt = recno; (w && cnt < N); ++cnt)
		if(write(STDOUT_FILENO,lines[cnt].l, 
		  lines[cnt].len)!= lines[cnt].len)
			err(1,"stdout: %s",strerror(errno));
	for (cnt = 0; cnt < recno; ++cnt)
		if(write(STDOUT_FILENO,lines[cnt].l, 
		  lines[cnt].len)!= lines[cnt].len)
			err(1,"stdout: %s",strerror(errno));
}
