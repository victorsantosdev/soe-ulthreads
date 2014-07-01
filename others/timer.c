#include <stdio.h>
#include <signal.h>
#include <sys/time.h>



void signal_handler(){
    printf("One second elapsed !\n");
}


int main(){

    struct itimerval time;

    time.it_interval.tv_sec = 1;
    time.it_interval.tv_usec = 0;
    time.it_value.tv_sec = 1;
    time.it_value.tv_usec = 0;

    signal(SIGVTALRM, signal_handler);

    setitimer(ITIMER_VIRTUAL, &time, NULL);

    while(1);

    return 0;

}


