#include"parson.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<pthread.h>
#include<unistd.h>
// 해시코드맵에 사용될 고정값
const int Z = 33;
// 해시테이블 배열의 크기
#define BUCKET_SIZE 31
//const int BUCKET_SIZE = 31; // 이거는 안됨 c++에서는 됨.:

typedef struct hash_node
{
    char* hash_key;
    char* data;
    struct hash_node* next;
}hash_node_t;

typedef struct hash_table
{
    struct hash_node* head;
    bool active_cell;
    int node_cnt;
}hash_table_t;

typedef struct json_message
{
    int thread_num;
    char** thread_name_list;
}json_t;

enum Status
{
    INIT,
    RUNNING,
    DONE,
    SLEEP
};

void init_bucket_array();
// 해시 함수 = 해시코드맵 + 압축맵
// 해시코드맵 : keys -> integers
unsigned long hash_code_map(char* key);
// 압축맵 : integers -> [0, BUCKET_SIZE-1] 인덱스화
int compression_map(unsigned long integers);
int hash_function(char* key);
void json_to_structure(json_t* jt);
void insert_hash_node(char* key, char* data);
void tls_func();
void* tls_worker_task(void* arg);
void* tls_boss_task(void* arg);
void print_and_save(char* thread_name);

json_t jt;
//FILE* csv_file;
__thread hash_table_t tls_ht[BUCKET_SIZE];// 동적할당 방식이 아니라 그냥 배열을 만듦
bool print_flag = false;
enum Status* status_array;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
    // 1. json 파일을 읽어서 스레드 설정을 읽어서 생성한다. csv 파일도 같이 읽어서 구조체에 저장.
    // -> json : (1) 스레드 개수, (2) 스레드 이름 리스트 저장.
    // -> csv : (1) key (2) data
/*
    csv_file = fopen("hash.csv", "r");
    if(csv_file == NULL)
    {
        perror("file 열기 실패\n");
        fclose(csv_file);
        exit(1);
    }
*/
    json_to_structure(&jt);    
    status_array = (enum Status*)malloc(sizeof(enum Status)*jt.thread_num);
    for(int i = 0; i < jt.thread_num; i++)
    {
        status_array[i] = INIT;
    }
    // 2. 스레드 이름을 파싱해서 각 스레드를 만들기
    // 3. 작업처리 : 근데 TLS를 어디에 적용해야함?
    // 각 스레드에서 작업시 해시테이블 생성용으로 만들어보기
    tls_func();
    

   // fclose(csv_file);// 파일 닫기.
    return 0;
}


void init_bucket_array()
{
    for(int i = 0; i < BUCKET_SIZE; i++)
    {
       tls_ht[i].head = NULL;
       tls_ht[i].active_cell = false;
       tls_ht[i].node_cnt = 0;
    }
}
// 해시코드맵 : keys -> integersv
unsigned long hash_code_map(char* key)
{
    unsigned long integers = 0;
    
    //printf("key = %s, key의 길이 = %d\n", key,(int)strlen(key)); 
    // 다항누적
    for(int i = 0; i < strlen(key); i++)
    { //  여기서 오버플로우가 자주 발생하므로 모듈로연산자를 통해서 해결하였다.
        // 버킷 사이즈(31) : 활성(22) 비활성(9)
        // 41 : 활성(23) 비활성(8)
        // 37 : 활성(25) 비활성(6)
        // 47 : 활성(22) 비활성 (9)
        // 43 : 24 7
        // 29 : 22 9  
        integers = (integers * Z + key[i]) % 37;
        integers = (integers * Z + key[i]) % 37;
    }
    //printf("integers = %ld\n", integers);
    return integers;
}
int compression_map(unsigned long integers)
{
    int M = BUCKET_SIZE;
    if( integers < 0 )
        integers = integers * -1;

    // 1. 나누기
    // |k|%M
   // int index = integers % M;
   // return index;

    // 2. 승합제
    // |ak + b| % M
    // a%M != 0 이여야 한다. 아니면 모든 정수가 동일한 값 b로 매핑됨.
    unsigned long a = 11;
    unsigned long b = 17;

    int index = (a*integers + b)%M;
    return index;
}


int hash_function(char* key)
{
    int integers = hash_code_map(key);
    return compression_map(integers);
}


void insert_hash_node( char* key, char* data)
{
    hash_node_t* new_node = (hash_node_t*)malloc(sizeof(hash_node_t));
    new_node->hash_key = (char*)malloc(sizeof(char)*(strlen(key)+1));
    new_node->data = (char*)malloc(sizeof(char)*(strlen(data)+1));
    strncpy(new_node->hash_key, key, strlen(key)+1);
    strncpy(new_node->data, data, strlen(data)+1);

    int index = hash_function(key);
    printf("index = %d, key = %s\n", index, key); 
    if(tls_ht[index].active_cell == false)
    {
       tls_ht[index].active_cell = true;
    }
    new_node->next = tls_ht[index].head;
    tls_ht[index].head = new_node;
    tls_ht[index].node_cnt += 1; // 노드 개수 증가
}

void json_to_structure(json_t* jt)
{
    JSON_Value* root_value;
    JSON_Object* root_object;

    root_value = json_parse_file("thread_info.json");
    root_object = json_value_get_object(root_value);

    int thread_num = (int)json_object_get_number(root_object, "thread_num");
    printf("스레드 개수 : %d\n", thread_num);

    if(thread_num < 1)
    {
        printf("ERROR 1\n");
        json_value_free(root_value);
        exit(1);
    }
    
    // thread에서 배열 가져오기
    JSON_Array* threads = json_object_get_array(root_object, "thread");
    if(threads == NULL)
    {
        printf("ERROR 2\n");
        json_value_free(root_value);
        exit(1);
    }
    
    // 구조체에 저장.
    jt->thread_num = thread_num;
    jt->thread_name_list = (char**)malloc(sizeof(char*)*thread_num);

    for(int i = 0; i < thread_num; i++)
    {
        JSON_Object* thread_obj = json_array_get_object(threads, i);
        if(thread_obj == NULL)
        {
            printf("ERROR 3\n");
            exit(1);
        }

        const char* thread_name = json_object_get_string(thread_obj, "name");
        if(thread_name == NULL)
        {
            printf("ERROR 4\n");
            exit(1);
        }
        
        int len = strlen(thread_name);
        jt->thread_name_list[i] = (char*)malloc(sizeof(char)*(len+1));
        strncpy(jt->thread_name_list[i], thread_name, len+1);

    }

}

void tls_func()
{
    int thread_num = jt.thread_num;
    pthread_t boss_thread;
    if(pthread_create(&boss_thread, NULL, tls_boss_task, NULL))
    {
        printf("boss thread 생성실패\n");
        exit(1);
    }
           
    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);
    for(int i = 0; i < thread_num; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&worker_threads[i], NULL, tls_worker_task, thread_id))
        {
            printf("worker thread 생성실패\n");
            exit(1);
        }
    }
    
    
    for(int i = 0; i < thread_num; i++)
    {
        pthread_join(worker_threads[i], NULL);
    }
   
    pthread_join(boss_thread, NULL);

    free(worker_threads);

}

void* tls_worker_task(void* arg)
{
    int thread_id = *((int*)arg);
    char* thread_name = jt.thread_name_list[thread_id];
    status_array[thread_id] = INIT;
    printf("thread name : %s |status : Initializing\n", thread_name);
    // 새로 추가한 파일포인터.
    FILE* file = fopen("hash.csv", "r");
    if(file == NULL)
    {
        perror("file 열기 실패\n");
        fclose(file);
        exit(1);
    }


    init_bucket_array();
    /*
    for(int i = 0; i < BUCKET_SIZE; i++)
    {
       tls_ht[i].head = NULL;
       tls_ht[i].active_cell = false;
       tls_ht[i].node_cnt = 0;
    }
   */ 
    char* line = NULL;
    char* save_ptr;
    size_t len = 0;
    ssize_t read;
    sleep(1);
    status_array[thread_id] = RUNNING;
    printf("thread name : %s |status : RUNNING\n", thread_name);
    sleep(1);

    while((read = getline(&line, &len, file)) != -1)
    {
        line[strcspn(line , "\n")] = 0;
        char* key = strtok_r(line , ",", &save_ptr);
        char* data = strtok_r(NULL, ",", &save_ptr);
        insert_hash_node(key, data);
    }

    status_array[thread_id] = DONE;
    printf("thread name : %s | status : DONE\n", thread_name);
    sleep(1);
    free(line);
    free(arg);
    fclose(file);
    status_array[thread_id] = SLEEP;

    //아래 while문이 실행되는지 확인할 것.
    while(!print_flag)
    {
        printf("thread %s | status : SLEEP\n", thread_name);
        sleep(1);
    }

    // 출력시작. 
    pthread_mutex_lock(&mutex);
    print_and_save(thread_name);
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);

}

void* tls_boss_task(void* arg)
{
    int cnt = 0;
    int thread_num = jt.thread_num;
    
    while( cnt < thread_num )
    {
        cnt = 0;
        for(int i = 0 ; i < thread_num; i++)
        {
            if(status_array[i] == DONE || status_array[i] == SLEEP)
            {
                printf("DONE 상태\n");
                cnt++;                            
            }
            else if(status_array[i] == SLEEP)
            {
                printf("sleep 상태\n");
                cnt++;
            }else if(status_array[i] == INIT)
            {
                printf("init 상태\n");
            }else
            {
                printf("running 상태\n");
            }
        }
        printf(" cnt : %d | thread_num : %d\n", cnt, thread_num);
        sleep(1);
    }
    print_flag = true;
    pthread_exit(0);
}

void print_and_save(char* thread_name)
{
    FILE* result_file = fopen("hash_result.txt", "a+");
    if(result_file == NULL)
    {
        perror("hash_result file 생성 실패\n");
        fclose(result_file);
        exit(1);
    }
    int node_cnt = 0;

    printf(" ============ %s hash_result ============== \n", thread_name);
    fprintf(result_file, " ============ %s hash_result ============== \n", thread_name);
    for(int i = 0; i < BUCKET_SIZE; i++)
    {
        printf("hash_array[%d], node_cnt = %d\n", i, tls_ht[i].node_cnt);
        fprintf(result_file, "hash_array[%d], node_cnt = %d\n", i, tls_ht[i].node_cnt);
        hash_node_t* current = tls_ht[i].head;

        while(current != NULL)
        {
            printf("%s -> ", current->hash_key);
            fprintf(result_file, "%s -> ", current->hash_key);
            current = current->next;
            node_cnt++;
        }
        printf("NULL\n\n");
        fprintf(result_file, "NULL\n\n");
    }
    
    int active = 0;
    int inactive = 0;

    for(int i = 0; i < BUCKET_SIZE; i++)
    {
        if(tls_ht[i].active_cell == true)
            active++;
        else
            inactive++;
    }
    printf("전체 노드수 : %d, 활성셀 수 = %d, 비활성셀 수 = %d\n", node_cnt, active, inactive);
    fprintf(result_file, "활성셀 수 = %d, 비활성셀 수 = %d\n", active, inactive);
    
    printf("======================================================\n\n\n");

    fprintf(result_file, "======================================================\n\n\n");
    
    fclose(result_file);
}
