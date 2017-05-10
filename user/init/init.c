#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


int factorial(int i){
    if (!i) return 1;
    else return i * factorial(i-1);
}

void systest(){
    asm volatile(
        "mov $42,%%rax;"
        "syscall": : :
        "%eax"
        );
}
void systesti(int i){
    (void)i;
    asm volatile(
        "mov $42,%%rax;"
        "syscall": : :
        "%eax"
        );
}


void thread(){
    printf("[th 2] Hey !\n");
    for(int i = 0 ; i < 20000000 ; ++i);
    printf("[th 2] will die now !\n");
    _texit(13);
}


int main(){
    printf("[Init] Init start\n");
    void* test = malloc(1024);

    pid_t t = clone(thread,(char*)test +1024);
    printf("[init] Created thread %d\n",t);

    for(int i = 0 ; i < 10000000 ; ++i);

    pid_t pid = fork();
    if(pid == 0){
        printf("First Process");
        for(int i = 0 ; i < 10000000 ; ++i);
        return 56;
    }
    pid = fork();
    if(pid == 0){
        printf("Second Process");
        for(int i = 0 ; i < 10000000 ; ++i);
        return 57;
        }

    int status;
    errno = 0;
    printf("[init] before wait\n",errno);
    wait(&status);
    printf("[init] errno : %lld and status %d\n",errno,status);
    printf("[init] before wait 2\n",errno);
    wait(&status);
    printf("[init] errno : %lld and status %d\n",errno,status);

    printf("[init] end of test");
    while(1);

    _texit(42);

    return factorial(5);
}
