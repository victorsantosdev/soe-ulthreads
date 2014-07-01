/** main preemptiva **/

#include "ulthread.h"

void thr_idle(int num) {
    while(1) {
        printf("Idle thread\n");
        waitfor(250);
    }
}

/*generic thread, just for test purpose*/
/*print int a, <a> times */
void thr_run(int a) {

    int times = a;
    while (times--) {
        printf("%d\n", a);
        waitfor(1000);
    }
    finish(a);
}

/*generic thread, just for test purpose*/ 
/*print int a, <a> times */ 
void thr_teste(int a) { 

    while (1) { 
        printf("%d\n", a);
        waitfor(1000);
    } 
    finish(a); 
} 

int main(void) {

    init();

    int res_a, res_b, res_c, res_d, res_e, res_f, res_g;

    int tid_a = create(&thr_teste, 10, THREAD_PRIORITY_NORMAL);
    int tid_b = create(&thr_teste, 15, THREAD_PRIORITY_NORMAL);
    int tid_c = create(&thr_idle, 8, THREAD_PRIORITY_LOWEST);
    //int tid_d = create(&thr_run, 5, THREAD_PRIORITY_ABOVE_NORMAL);
    //int tid_e = create(&thr_run, 7, THREAD_PRIORITY_BELOW_NORMAL);
    //int tid_f = create(&thr_run, 2, THREAD_PRIORITY_NORMAL);
    //int tid_g = create(&thr_run, 3, THREAD_PRIORITY_TIME_CRITICAL);
    print_list();
    debug();

    join(tid_a, &res_a);
    join(tid_b, &res_b);
    //join(tid_c, &res_c);
    
    //join(tid_d, &res_d);
    //join(tid_e, &res_e);
    //join(tid_f, &res_f);
    //join(tid_g, &res_g);

    printf("Both threads have finished with status %d and %d\n", res_a, res_b);
    printf("I'm down too... bye bye!\n");
    return 0;
}






