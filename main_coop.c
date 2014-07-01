#include "ulthread.h"
#include <unistd.h>

void wait_ms(int miliseconds) {
    usleep(miliseconds * 1000); //passing to miliseconds
}

void thr_idle(int num) {
    while(1) {
        printf("Idle thread\n");
        yield();
        wait_ms(500);
    }
}

/*generic thread, just for test purpose*/
/*print int a, <a> times */
void thr_run(int a) {
    
    int times = a;
    while (times--) {
        printf("%d\n", a);
        /*free cpu to run another thread with yield method()*/
        yield();
    }
    finish(a);
}

int main(void) {

    init();
    
    int res_a, res_b, res_c, res_d, res_e, res_f, res_g;

    int tid_a = create(&thr_run, 10, THREAD_PRIORITY_NORMAL);
    int tid_b = create(&thr_run, 15, THREAD_PRIORITY_NORMAL);
    int tid_c = create(&thr_idle, 8, THREAD_PRIORITY_LOWEST);
    int tid_d = create(&thr_run, 5, THREAD_PRIORITY_ABOVE_NORMAL);
    int tid_e = create(&thr_run, 7, THREAD_PRIORITY_BELOW_NORMAL);
    int tid_f = create(&thr_run, 2, THREAD_PRIORITY_NORMAL);
    int tid_g = create(&thr_run, 3, THREAD_PRIORITY_TIME_CRITICAL);

    print_list();
#if 1
    join(tid_a, &res_a);
    /*waiting until thread a returns*/
    join(tid_b, &res_b);
    join(tid_c, &res_c);
    join(tid_d, &res_d);
    join(tid_e, &res_e);
    join(tid_f, &res_f);
    join(tid_g, &res_g);

   printf("Both threads have finished with status %d and %d\n", res_a, res_b);
   printf("I'm donw too... bye bye!\n");
#endif

    return 0;
}






