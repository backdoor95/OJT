#include"parson.h"
#include<time.h>
#include<pthread.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

// 1. Json 파일을 읽어서 구조체에 저장
// 2. 스레드 요구사항에 따라서 생성
// 3. Json 파일 요구사항 동작후 Report 구조체에 저장
//      1) repeat_cnt 
//      2) repeat : 랜덤 문자열
//      3) 구조체에 저장
// 4. 구조체에 저장되어 있는 정보를 JSON으로 변환뒤, 파일로 저장& 출력


const char charSet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int charSetLen = 52;

typedef struct _THREAD
{
    int repeat_cnt;// 스레드에 할당된 반복 횟수.
    char* name;// random한 문자열의 최대길이는 알파벳 100개까지
}thread_t;

typedef struct _JSON_MESSAGE
{
    int repeat_cnt;
    int thread_num;
    Thread* threadList;		
}json_t;

typedef struct _REPORT
{

    int repeat_cnt;
    char**randomStringArray;

}report_t;

void generate_random_string(char*str);
void create_threads(json_t* j);
void* thread_task(void* arg);
void json_to_structure(json_t* j);
void structure_to_json(json_t* j);
void thread_timechecker();
int main(void)
{
    srand((unsigned int)time(NULL));
    json_t jt;

    // 1. json에서 작업명세서 내용 파싱후 구조체에 저장.
    // 실패시 에러 메시지 출력후 프로그램 종료
    json_to_structure(&jt);

    // 2. thread 생성 및 관리 함수 
    create_threads(&jt);


    return 0;
}

void create_threads(json_t* jt)
{
    int thread_cnt = jt->thread_cnt;
    int repeat_cnt = jt->thread_cnt;

    // thread 메모리 할당.
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * task_count);
    for(int i = 0; i < thread_cnt ; i++)
    {
        if(pthread_create(&threads[i], NULL, thread_task, (void*)&repeat_cnt))
        {
            printf("스레드 생성 실패!!!\n");
            return 1;
        }
    }
    // 완료 될때까지 대기.
    for( int i = 0; i < thread_cnt; i++)
    {
        pthread_join(threads[i], NULL);
    }   
    free(threads);

}

// json에서 받은 설정값 대로 작동.
void* thread_task(void* arg)
{
    int* repeat_num = (int*)arg;
  
    
    for(int i=0 ;i<*repeat_num;i++)
    {
        i
    }


    pthread_exit(NULL);
}

void json_to_structure(json_t* jt)
{
    JSON_Value *rootValue;
    JSON_Object *rootObject;

    /* 초기화 */
    rootValue = json_parse_file("jparser2.json");      // JSON 파일을 읽어서 파싱 
    rootObject = json_value_get_object(rootValue);    // JSON_Value에서 JSON_Object를 얻음.

    // repeat, thread_num 가져오기
    int repeat_cnt = (int)json_object_get_number(rootObject, "repeat");
    int thread_num = (int)json_object_get_number(rootObject, "thread_num");

    printf("repeat: %d\n", repeat_cnt);
    printf("thread_num: %d\n", thread_num);


    if(thread_num < 1)
    {
        printf("ERROR\n"); 
        json_value_free(rootValue);
        exit(1);
    }

    // thread에서 배열 가져오기
    JSON_Array *threads = json_object_get_array(rootObject, "thread");
    if(threads == NULL)
    {
        printf("thread name 객체parsing 실패.\n");
        json_value_free(rootValue);
        exit(1);
    }

    //===================== 1. 구조체에 저장========================//
    j->repeat_cnt = repeat_cnt;
    j->thread_num = thread_num;
    j->threadList = (Thread*)malloc(sizeof(Thread)*thread_num);

    // 배열 크기 확인
    size_t threadCount = json_array_get_count(threads);

    // 각 thread 객체를 순회하면서 처리
    for (size_t i = 0; i < threadCount; i++) {
        JSON_Object *threadObj = json_array_get_object(threads, i);
        if (threadObj == NULL) {
            printf("Error : parsing 실패 !!! thread 배열index %zu 오류발생.\n", i);
            exit(1); // 오류 발생 시 error 메시지 출력후 프로그램 종료
        }

        const char *name = json_object_get_string(threadObj, "name");
        if (name == NULL) {
            printf("Error : Thread 배열의 인덱스%zu번에서 name의 value가 NULL입니다.\n", i);
            exit(1);
        } else {
            printf("Thread %zu name: %s\n", i, name);
            // strcpy(dest, src) 함수 사용
            // 검증 필요함. 
            int len = strlen(name);
            // 뒤에 NULL이 들어가야 하므로 +1 해줘야함.
            j->threadList[i].name = (char*)malloc(sizeof(char)*len+1);
            strcpy(j->threadList[i].name, name);// Thread 이름을 저장.

        }

    }

}

// 여기서 json을 토대로 만든 구조체를 json 파일로 생성
void structure_to_json(JsonMessage *j)
{

}

// 랜덤 문자열 반환.
void generate_random_string(char* str)
{
    // random 문자열의 길이는 랜덤으로 정해짐.
    int size = rand()%101+1;
    char* str = (char*)malloc(size+1);

    for(int i=0;i<size;i++)
    {
        str[i]=charSet[rand()%charSetLen];
    }

    str[size] = '\0';
}

void thread_timechecker()
{
    return;
}
