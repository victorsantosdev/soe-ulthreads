#include "ulthread.h"

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
    
    int res_a, res_b;

    int tid_a = create(&thr_run, 10);
    int tid_b = create(&thr_run, 15);

    join(tid_a, &res_a);
    /*waiting until thread a returns*/
    join(tid_b, &res_b);

   printf("Both threads have finished with status %d and %d\n", res_a, res_b);
   printf("I'm donw too... bye bye!\n");


    return 0;
}






