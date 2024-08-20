#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

void* thread_function(void* arg)
{
    int *num = (int*)arg;
    printf("Thread 숫자 : %d\n", *num);
    pthread_exit(NULL);
}
 
int main() {
    pthread_t thread_id; // 스레드 ID
    int num = 5;

    // 스레드 생성
    if (pthread_create(&thread_id, NULL, thread_function, (void*) &num) != 0) {
        printf("Failed to create thread\n");
        return 1;
    }
    // pthread_create 함수에서 &num은 함수의 인자를 의미한다. 

    // 스레드가 종료될 때까지 대기
    if (pthread_join(thread_id, NULL) != 0) {
        printf("Failed to join thread\n");
        return 2;
    }

    printf("Thread has finished execution\n");

    return 0;
}


