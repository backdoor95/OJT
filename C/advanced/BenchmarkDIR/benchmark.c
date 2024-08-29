#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>
#include<unistd.h>

const int NUM_THREADS = 8;
const int WORK_QUOTA = 2000000000;

int time_check = 0;
long long global_goal = 16000000000;
long long global_count = 0;
bool stop = false;
__thread long long tls_var = 0;
clock_t start_time, end_time;
double timer;
double time_gap = 1.0;

long long report[8*8];// THREADS_COUNT*THREADS_COUNT 해도 되는지 질문 -> 캐시에 영향있나?
// 로컬변수에 그냥 값을 저장한 뒤에 그냥 report 배열에 넣으면 되는것 아닌가?
void check_task();
void show_report();
void init_var();
void* shared_threadtask(void* arg);
void* local_threadtask(void* arg);
// tls :  worker, boss 
void* tls_worker_threadtask(void* arg);
void* tls_boss_threadtask(void* arg);

void shared_var_func();
void local_var_func();
void tls_var_func();
void atomic_shared_func();
void atomic_local_func();


int main(void)
{
    // 1. 공유 변수 멀티스레딩.
    shared_var_func();
    // 2. 지역 변수 멀티스레딩.
   // local_var_func();
    // 3. TLS
    //tls_var_func();
    // 4. atomic(shared)
    //atomic_shared_func();    
    // 5. atomic(local)
    //atomic_local_func();
    // 6. spinlock

    // 7. semaphore



    return 0;
}

void local_var_func()
{
    printf("---------------------------local var 멀티스레딩 벤치마크-----------------------\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);

    //printf("seq 1\n"); 

    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&threads[i], NULL, local_threadtask, thread_id))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }

    while(1)
    { 
        show_report();
        sleep(1);
        if(global_count >= global_goal)
            exit(0);
    }

    // 완료 될 때까지 대기.
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(threads[i], NULL);
    }
    //printf(" benchmark running time = %.10fs\n", (double)(end_time - start_time));
    //  메모리 해제
    free(threads);
    show_report();
    // 배열 0으로 초기화.
    for (size_t i = 0; i < 8*8; i++)
    {
        report[i] = 0;
    }

    printf("===================================================================================");
    printf("\n\n\n");
    return;

}

void shared_var_func()
{
    printf("---------------------------shared var 멀티스레딩 벤치마크-----------------------\n");
    // 원하는 스레드 개수만큼 메모리 할당.
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    
    start_time = clock(); 

    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&threads[i], NULL, shared_threadtask, thread_id))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }
    // 만약에 스레드가 종료되었다는 메시지가 올라간다면?
    // 여기 while(1)때문에 진행이 될까, 안될까?

    while(1)
    { 
        printf("global_count : %lld", global_count);
        sleep(1);
        if(global_count >= global_goal)
            exit(0);
    }


    // 완료 될 때까지 대기.
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(threads[i], NULL);
    }

    //printf(" benchmark running time = %.10fs\n", (double)(end_time - start_time));
    //  메모리 해제
    free(threads);
    //show_report();
    // 배열 0으로 초기화.
    for (size_t i = 0; i < 8*8; i++)
    {
        report[i] = 0;
    }
    init_var();
    printf("===================================================================================");
    printf("\n\n\n");
    return;
}

void tls_var_func()
{
    printf("---------------------------tls var 멀티스레딩 벤치마크-----------------------\n");
    // 원하는 worker 스레드 개수만큼 메모리 할당.
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);
    clock_t start_time, end_time;
    start_time = time(NULL); 

    for(int i = 0; i < NUM_THREADS; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;

        if(pthread_create(&worker_threads[i], NULL, tls_worker_threadtask, thread_id))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }

    // 완료 될 때까지 대기.
    for( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        pthread_join(worker_threads[i], NULL);
    }

    end_time = time(NULL);
    //  메모리 해제
    free(worker_threads);
    show_report();
    // 배열 0으로 초기화.
    for (size_t i = 0; i < 8*8; i++)
    {
        report[i] = 0;
    }
    printf("===================================================================================");
    printf("\n\n\n");
    return;
}

void atomic_shared_func()
{
    
}

void* local_threadtask(void* arg)
{
    int thread_id = *((int*)arg);
    long long local_var = 0;

    while(1)
    {
       report[thread_id*8] = ++local_var;
    }

   // report[thread_id*8] = local_var;
    free(arg);
    pthread_exit(0);
}

void* shared_threadtask(void* arg)
{
    int thread_id = *((int*)arg);
    start_time = clock();

    while(1)
    {
        //global_count = global_count + 1;
        ++global_count;
        end_time = clock();
        timer = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        if(timer >= 2.0)
            break;
    }
    printf(" global shared var = %lld\n", global_count);
    free(arg);
    pthread_exit(0);
}

void* tls_worker_threadtask(void* arg)
{
    int thread_id = *((int*)arg);
    //printf(" t_id = %d\n",thread_id);
    
    start_time = clock();
    double timer;

    while(1)
    {
        ++(tls_var);
        end_time = clock();
        timer = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
        if(timer >= 2.0)
            break;    
    }
    
    report[thread_id*8] = tls_var;
    free(arg);
    pthread_exit(0);
}

void* tls_boss_threadtask(void* arg)
{
    while(true)
    {
        for(int i = 0; i< NUM_THREADS; i++)
        {
            tls_var += report[i*8];// i * NUM_THREADS
        }
        //printf("boss tls_var = %lld\n", tls_var);
        if(tls_var >= global_goal)
        {
            printf(" 목표 달성 boss tls_var = %lld \n", tls_var);
            stop = true;
            break;
        }
        tls_var = 0;
    }
    pthread_exit(0);
}

void init_var()
{
    global_count = 0;
    return;
}
void check_task()
{
    long long sum = 0;
    for( int i = 0; i < NUM_THREADS; i++)
    {
        sum += report[i*8];
    }

    if(sum > global_goal)
    {
        end_time = clock();
        printf("running time = %lf\n", ((double)(end_time-start_time))/CLOCKS_PER_SEC); 
        exit(0);
    }
   
   return;
}
void show_report()
{
    long long sum = 0;
    for( int i = 0; i < NUM_THREADS; i++)
    {
        printf(" report[%d] = %lld,", i, report[i*8]);
        sum += report[i*8];
    }
    printf("\n");
    time_check++;
    printf("work sum = %lld\n", sum);
    if(sum > global_goal)
    {
        printf(" == 출력 횟수 : %d  ==\n", time_check);
        exit(0);
    }
    return;
}
