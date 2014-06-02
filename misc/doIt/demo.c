/*
 * Multithread demo program.
 * OS course.
 *
 * This program demonstrates context switching and preemption.
 * Check out `man setjmp', `man longjump' and the following
 * files in /usr/include: setjmp.h,jmpbuf.h,unistd.h (for
 * usleep and ualarm). It will help if you can dig out
 * some more documentation on setjmp and longjmp.
 *
 */

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>

#define SECOND 1000000
 
char stack1[4096];
char stack2[4096];

jmp_buf jbuf[2];

f()
{
  int i=0;
  while(1){
    printf("in f: %d\n",i++);
    usleep(SECOND);
  }
}

g()
{
  int i=0;
  while(1){
    printf("in g: %d\n",i++);
    usleep(SECOND);
  }
}


setup()
{
  sigsetjmp(jbuf[0],1);
  jbuf[0][0].__jmpbuf[JB_SP] = (unsigned) stack1 + 4096;  
  jbuf[0][0].__jmpbuf[JB_PC] = (unsigned) f; 
  
  sigsetjmp(jbuf[1],1);
  jbuf[1][0].__jmpbuf[JB_SP] = (unsigned) stack2 + 4096;  
  jbuf[1][0].__jmpbuf[JB_PC] = (unsigned) g; 

}

static toggle = 0;

void dispatch(int sig)
{
  puts("in dispatch");
  if (sigsetjmp(jbuf[toggle],1) == 1)
    return;

  puts("DISPATCH");
  toggle = 1 - toggle;
  siglongjmp(jbuf[toggle],1);
}


main()
{
  signal(SIGALRM, dispatch);
  setup();		
  ualarm(5*SECOND, 5*SECOND);
  longjmp(jbuf[0],1);	
 }
