#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<stdbool.h>
#include<unistd.h>
#include<stdatomic.h>
#include<signal.h>
#include<semaphore.h>

#define NUM_THREADS 8
const int WORK_QUOTA = 2000000000;

__thread long long tls_var = 0;
__thread long long tls_report_array[NUM_THREADS];
atomic_long atomic_shared_var = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;// mutex
pthread_spinlock_t spin_lock;// spinlock
sem_t sem; // 세마포어


bool stop = false;
int runtime_check = 0;
long long global_goal = 16000000000;
long long global_count = 0;
long long global_sum = 0;
long long report[8*8];

void sigint_handler(int sig);
void show_report();
void* shared_worker_task(void* arg);
void* shared_boss_task(void* arg);
void shared_func();
void* local_worker_task(void* arg);
void* local_boss_task(void* arg);
void local_func();
void* tls_worker_task(void* arg);
void* tls_boss_task(void* arg);
void tls_func();
void atomic_shared_func();
void* atomic_shared_worker_task(void* arg);
void* atomic_shared_boss_task(void* arg);
void atomic_local_func();
void* atomic_local_worker_task(void* arg);
void* atomic_local_boss_task(void* arg);
void mutex_func();
void* mutex_worker_task(void* arg);
void* mutex_boss_task(void* arg);
void semaphore_func();
void* semaphore_worker_task(void* arg);
void* semaphore_boss_task(void* arg);
void spinlock_func();
void* spinlock_worker_task(void* arg);
void* spinlock_boss_task(void* arg);
void tls_array_func();
void* tls_array_worker_task(void* arg);
void* tls_array_boss_task(void* arg);



int main()
{
    // 1
    //shared_func();
    // 2
    //local_func();
    // 3
    //tls_func();
    // 4
    //atomic_shared_func();
    // 5
    //atomic_local_func();
    // 6 mutext는 정적 초기화를 함. PHTREAD_MUTEX_INITIALIZER
    mutex_func();
    // 7
    //sem_init(&sem, 0, 1);
    //semaphore_func();
    // 8
    //pthread_spin_init(&spin_lock,0);
    //spinlock_func();
    //pthread_spin_destroy(&spin_lock);
    // 9
    //tls_array_func();
    
    return 0;
    
}

void shared_func()
{
    printf("===========================[shared var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        if(pthread_create(&worker_threads[i], NULL, shared_worker_task, NULL))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }
    //int count = 0;
    signal(SIGINT, sigint_handler);
    pthread_t boss_thread;
    pthread_create(&boss_thread, NULL, shared_boss_task, NULL);
    //while(1)
    //{
    //    sleep(1);
    //    printf("global_var : %lld\n", global_count);
    //    runtime_check++;
    //}
    //printf("running time : %d\n", count);
    // 완료 될 때까지 대기.

    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    global_count = 0;
    stop = false;
    printf("===================================================================================");
    printf("\n\n\n");
}

void* shared_worker_task(void* arg)
{
    while(1)// while(1)
    { 
        global_count++;
        //if(global_count >= global_goal)
            //break;
    }
    //stop = true;
    pthread_exit(0);
}

void* shared_boss_task(void*arg)
{
    while(1)
    {
        sleep(1);
        runtime_check++;    
        printf("global_count : %lld\n", global_count);
    }
    pthread_exit(0);
}

void local_func()
{
    printf("===========================[local var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&threads[i], NULL, local_worker_task, thread_id))
        {
            printf("worker 스레드 생성 실패!!!\n");
            exit(1);
        }
    }
    
    signal(SIGINT, sigint_handler);

    pthread_t boss_thread;
    if(pthread_create(&boss_thread, NULL, local_boss_task, NULL))
    {
        printf("boss 스레드 생성 실패!!\n");
        exit(1);
    }

//     int count = 0;
//     while(1)
//     {
//         sleep(1);
//         show_report();
//         count++;
//         //sleep(1);
//     }
//     printf("running time : %d\n", count);
//#     
    // 완료 될 때까지 대기.
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    stop = false;
    printf("===================================================================================");
    printf("\n\n\n");

}
void* local_worker_task(void* arg)
{
    int thread_id = *((int*)arg);
    long long local_var = 0;

    while(1)// while(1) vs while(!stop)
    {
       report[thread_id*8] = ++local_var;
      // if(stop == true)
      // {
        //   free(arg);
         //  pthread_exit(0);
      // }
    }
   // free(arg);
   // pthread_exit(0);
}
void* local_boss_task(void* arg)
{
    while(1)
    {
        sleep(1);
        runtime_check++;
        show_report();
    }
}
void tls_func()
{

    printf("===========================[tls var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&worker_threads[i], NULL, tls_worker_task, thread_id))
        {
            printf("worker thread 생성 실패!!!\n");
            exit(1);
        }
    }
    
    signal(SIGINT, sigint_handler);

    pthread_t boss_thread;
    if(pthread_create(&boss_thread, NULL, tls_boss_task, NULL))
    {
        printf("boss thread 생성 실패\n");
        exit(1);
    }
   // 완료 될 때까지 대기.
    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    //stop = false;
    printf("===================================================================================");
    printf("\n\n\n");


}

void* tls_worker_task(void* arg)
{
    int thread_id = *((int*)arg);

    while(1)
    {
       report[thread_id*8] = ++(tls_var);
    }
    free(arg);
    pthread_exit(0);
}

void* tls_boss_task(void* arg)
{
    while(1)
    {
        sleep(1);
        show_report();
        runtime_check++;
    }
    //printf("running time : %lld\n", tls_var);
    pthread_exit(0);
}


void atomic_shared_func()
{

    printf("===========================[atomic_shared var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&worker_threads[i], NULL, atomic_shared_worker_task, thread_id))
        {
            printf("worker thread 생성 실패!!!\n");
            exit(1);
        }
    }

    signal(SIGINT, sigint_handler);

    pthread_t boss_thread;
    if(pthread_create(&boss_thread, NULL, atomic_shared_boss_task, NULL))
    {
        printf("boss thread 생성 실패\n");
        exit(1);
    }
   // 완료 될 때까지 대기.
    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    stop = false;
    printf("===================================================================================");
    printf("\n\n\n");


}

void* atomic_shared_worker_task(void* arg)
{
    int thread_id = *((int*)arg);
    while(1)
    { 
        atomic_shared_var++;
        //if(atomic_shared_var++ >= global_goal)
            //break;
    }
    //stop = true;
    free(arg);
    pthread_exit(0);
}

void* atomic_shared_boss_task(void* arg)
{
    while(1)
    {
        sleep(1);
        runtime_check++;
        printf("atomic_shared_var = %ld\n", atomic_shared_var);
    }
    free(arg);
    pthread_exit(0);
}

void atomic_local_func()
{

    printf("===========================[atomic_local var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&worker_threads[i], NULL, atomic_shared_worker_task, thread_id))
        {
            printf("worker thread 생성 실패!!!\n");
            exit(1);
        }
    }

    signal(SIGINT, sigint_handler);

    pthread_t boss_thread;
    if(pthread_create(&boss_thread, NULL, atomic_shared_boss_task, NULL))
    {
        printf("boss thread 생성 실패\n");
        exit(1);
    }
   // 완료 될 때까지 대기.
    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    stop = false;
    printf("===================================================================================");
    printf("\n\n\n");


}

void* atomic_local_worker_task(void* arg)
{
    int thread_id = *((int*)arg);
    atomic_long atomic_local_var = 0;
    
    while(1)
    { 
        report[thread_id*8] = ++atomic_local_var;
    }
    free(arg);
    pthread_exit(0);
}

void* atomic_local_boss_task(void* arg)
{
    while(1)
    {
        sleep(1);
        runtime_check++;
        show_report();
    }
    pthread_exit(0);
}

void mutex_func()
{
    printf("===========================[mutex var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        if(pthread_create(&worker_threads[i], NULL, mutex_worker_task, NULL))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }
    //int count = 0;
    signal(SIGINT, sigint_handler);
    pthread_t boss_thread;
    pthread_create(&boss_thread, NULL, mutex_boss_task, NULL);
    //while(1)
    //{
    //    sleep(1);
    //    printf("global_var : %lld\n", global_count);
    //    runtime_check++;
    //}
    //printf("running time : %d\n", count);
    // 완료 될 때까지 대기.

    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    global_count = 0;
    stop = false;
    printf("===================================================================================");
    printf("\n\n\n");
}

void* mutex_worker_task(void* arg)
{
    while(1)// while(1)
    {
        pthread_mutex_lock(&mutex); 
        global_count++;
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

void* mutex_boss_task(void* arg)
{
    while(1)
    {
        sleep(1);
        runtime_check++;
        printf("global_count : %lld\n", global_count);
    }
    pthread_exit(0);
}

void semaphore_func()
{
    printf("===========================[semaphore var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        if(pthread_create(&worker_threads[i], NULL, semaphore_worker_task, NULL))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }
    //int count = 0;
    signal(SIGINT, sigint_handler);
    pthread_t boss_thread;
    pthread_create(&boss_thread, NULL, semaphore_boss_task, NULL);
    //while(1)
    //{
    //    sleep(1);
    //    printf("global_var : %lld\n", global_count);
    //    runtime_check++;
    //}
    //printf("running time : %d\n", count);
    // 완료 될 때까지 대기.

    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    global_count = 0;
    stop = false;
    printf("===================================================================================");
    printf("\n\n\n");
}

void* semaphore_worker_task(void* arg)
{
    while(1)
    {
        sem_wait(&sem);
        global_count++;
        sem_post(&sem);
    }
    pthread_exit(0);
}

void* semaphore_boss_task(void* arg)
{
    while(1)
    {
        sleep(1);
        runtime_check++;
        printf("global_count : %lld\n", global_count);
    }
}

void spinlock_func()
{
    printf("===========================[spin var]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        if(pthread_create(&worker_threads[i], NULL, spinlock_worker_task, NULL))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }
    signal(SIGINT, sigint_handler);
    pthread_t boss_thread;
    pthread_create(&boss_thread, NULL, spinlock_boss_task, NULL);
    //while(1)
    //{
    //    sleep(1);
    //    printf("global_var : %lld\n", global_count);
    //    runtime_check++;
    //}
    //printf("running time : %d\n", count);
    // 완료 될 때까지 대기.

    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    global_count = 0;
    stop = false;
    printf("===================================================================================");
    printf("\n\n\n");
}

void* spinlock_worker_task(void* arg)
{
    while(1)
    {
       pthread_spin_lock(&spin_lock);
       global_count++;
       pthread_spin_unlock(&spin_lock); 
    }
    pthread_exit(0);    
}

void* spinlock_boss_task(void* arg)
{
    while(1)
    {
        sleep(1);
        runtime_check++;
        printf("global_count : %lld\n", global_count);
    }
}

void tls_array_func()
{

    printf("===========================[tls var and tls array]=================================");
    printf("\n\n\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&worker_threads[i], NULL, tls_array_worker_task, thread_id))
        {
            printf("worker thread 생성 실패!!!\n");
            exit(1);
        }
    }
    
    signal(SIGINT, sigint_handler);

    pthread_t boss_thread;
    if(pthread_create(&boss_thread, NULL, tls_array_boss_task, NULL))
    {
        printf("boss thread 생성 실패\n");
        exit(1);
    }
   // 완료 될 때까지 대기.
    pthread_join(boss_thread, NULL);
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
    //stop = false;
    printf("===================================================================================");
    printf("\n\n\n");



}

void* tls_array_worker_task(void* arg)
{
    int thread_id = *((int*)arg);
    int index = thread_id * 8;
    while(1)
    {
       report[index] = ++(tls_var);
    }
    free(arg);
    pthread_exit(0);
}

void* tls_array_boss_task(void* arg)
{

}

void show_report()
{
    global_sum = 0;
    for( int i = 0; i < NUM_THREADS; i++)
    {
        //printf(" report[%d] = %lld,", i, report[i*8]);
        global_sum += report[i*8];
    }
    //printf("\n");
    printf("work sum = %lld\n", global_sum);
//    if(sum >= global_goal)
//    {
//        stop = true;
//    }
    return;
}

void sigint_handler(int sig)
{
    printf("\n running time : %d\n", runtime_check);
    printf("\n 연산예상시간 : %lfs\n", (double)(global_goal * runtime_check)/(global_sum)); 
    exit(0);
}


