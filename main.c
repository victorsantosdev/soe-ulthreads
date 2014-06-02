#include "ulthread.h"

void thr_run(int a) {
    unsigned int turn = 0;
    while (1) {
        printf("%d\n", a);
        yield();
    }
    finish(0);
}

int main(void) {

    init();
    int tid_a = create(&thr_run, 10);
    int tid_b = create(&thr_run, 15);

    while(1) {
        printf("A\n");
        yield();
    }

    return 0;
}






