#include <stdio.h>
#include "ul_threads.h"

void busywait(unsigned int n)
{
    for (; n > 0; n--); 
}

void func_a(int id){
    int i = 0;
    for( ; i <50; i++){
        printf(" -=A%d=- ", getid());
        busywait(0x1FFFFF);
    }

    finish_thread(id);
}

void func_b(int id){
    int i = 0;
    for( ; i <50; i++){
        printf(" -=B%d=- ", getid());
        busywait(0x1FFFFF);
    }
    finish_thread(id);
}

void func_c(int id){
    int i = 0;
    for( ; i <10; i++){
        printf(" -=C%d=- ", getid());
        yield();
    }
    finish_thread(id);
}

int main()
{
    printf("User-level Threads Scheduler\n");
    setvbuf(stdout, 0, _IONBF, BUFSIZ);

    init();

    printf("Hello! I'm the main thread, my ID is %d.\n", getid());
    printf("Now I`ll create another 3 threads and wait their jobs finish!\n");

    int thrA = create(&func_a, 1979);
    int thrB = create(&func_b, 2008);
    int thrC = create(&func_c, 29);

    int retA, retB, retC;

    join(thrB, &retB);
    join(thrA, &retA);
    join(thrC, &retC);

    printf("\n\n");
    printf("Thread A returned %d\n", retA);
    printf("Thread B returned %d\n", retB);
    printf("Thread C returned %d\n", retC);

    printf("I`m also done ! Bye ...\n");

    return 0;
}


