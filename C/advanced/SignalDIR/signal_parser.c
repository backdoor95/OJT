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
    thread_t* threadList;		
}json_t;

// 굳이 report 구조체가 필요하나
// 그냥 thread 구조체만으로 가능할듯 한데 -> 나중에 리펙토링 고려해보기.
typedef struct _REPORT
{
    int repeat_cnt;
    char* thread_name;
    char** randomStringArray;
}report_t;

void generate_random_string(char** tr);
void create_threads(json_t* j);
void* thread_task(void* arg);
void json_to_structure(json_t* j);
void structure_to_json(report_t* rt);
int main(void)
{
    srand((unsigned int)time(NULL));
    json_t jt;

    // 1. json에서 작업명세서 내용 파싱후 구조체에 저장.
    // 실패시 에러 메시지 출력후 프로그램 종료
    json_to_structure(&jt);

    // 2. thread 생성 및 관리 함수 
    create_threads(&jt);

    // 3. 전체 작업 완료 출력
    printf("전체 작업을 완료하였습니다.\n");

    return 0;
}

void create_threads(json_t* jt)
{
    int thread_cnt = jt->thread_num;
    int repeat_cnt = jt->repeat_cnt;

    // thread 메모리 할당.
    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_cnt);
    for(int i = 0; i < thread_cnt ; i++)
    {
        if(pthread_create(&threads[i], NULL, thread_task, (void*)&(jt->threadList[i])))
        {
            printf("스레드 생성 실패!!!\n");
            exit(1);
        }
    }
    // 완료 될때까지 대기.
    for( int i = 0; i < thread_cnt; i++)
    {
        pthread_join(threads[i], NULL);
    }   
    // 메모리 해제 
    free(threads);
    
}

// json에서 받은 설정값 대로 작동+ 매개변수 출력 및 타이머 역할 함수
void* thread_task(void* arg)
{
    thread_t* worker_thread = (thread_t*)arg;
    
    report_t rt;
    rt.thread_name = worker_thread->name;
    rt.repeat_cnt = worker_thread->repeat_cnt;
    rt.randomStringArray = (char**)malloc( sizeof(char*) * rt.repeat_cnt);
    
    time_t start_time, current_time;
    start_time = time(NULL);

    for(int i=0 ; i < rt.repeat_cnt; i++)
    {
        generate_random_string(&(rt.randomStringArray[i]));
        current_time = time(NULL);
        printf("%s running time : %d\n", worker_thread->name, (int)difftime(current_time,start_time));
       
        // 1초 대기 
        sleep(1);
    }

    // json 파일 생성
    structure_to_json(&rt);
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
    jt->repeat_cnt = repeat_cnt;
    jt->thread_num = thread_num;
    jt->threadList = (thread_t*)malloc(sizeof(thread_t)*thread_num);

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
            // 1. repeat 횟수 저장.
            jt->threadList[i].repeat_cnt = repeat_cnt;
            // 2. thread 이름저장
            printf("Thread %zu name: %s\n", i, name);
            // strcpy(dest, src) 함수 사용
            // 검증 필요함. 
            int len = strlen(name);
            // 뒤에 NULL이 들어가야 하므로 +1 해줘야함.
            jt->threadList[i].name = (char*)malloc(sizeof(char)*len+1);
            strcpy(jt->threadList[i].name, name);// Thread 이름을 저장.

        }

    }

}

// 여기서 report_t 구조체를 json 파일로 생성
void structure_to_json(report_t* rt)
{
    JSON_Value *root_value;
    JSON_Object *root_object;

    /* 초기화 */
    root_value = json_value_init_object();             // JSON_Value 생성 및 초기화
    root_object = json_value_get_object(root_value);    // JSON_Value에서 JSON_Object를 얻음

    // "repeat_cnt" 설정
    int repeat_cnt = rt->repeat_cnt;
    json_object_set_number(root_object, "repeat_cnt", repeat_cnt);

    // "repeat" 배열 생성
    JSON_Value *repeat_array_value = json_value_init_array();
    JSON_Array *repeat_array = json_value_get_array(repeat_array_value);
    json_object_set_value(root_object, "repeat", repeat_array_value);

    // 반복해서 랜덤 문자열을 생성하여 배열에 추가
    for (int i = 0; i < repeat_cnt; i++) {
        // 각 "random_string"을 JSON 객체로 추가
        JSON_Value *repeat_object_value = json_value_init_object();
        JSON_Object *repeat_object = json_value_get_object(repeat_object_value);
        json_object_set_string(repeat_object, "random_string", rt->randomStringArray[i]);

        // 배열에 추가
        json_array_append_value(repeat_array, repeat_object_value);
    }
    
    // 파일 이름에 스레드 이름 같이 넣어서 만들기
    char filename[120];
    char suffix[] = "_report.json";
    strcpy(filename, rt->thread_name);
    strcat(filename, suffix); 

    // JSON 데이터를 파일로 저장
    if (json_serialize_to_file_pretty(root_value, filename) == JSONSuccess) {
        printf("[파일명 : %s] json 파일 생성 success\n", filename);
    } else {
        printf("json 파일 생성 fail\n");
    }

    // JSON 객체 메모리 해제
    json_value_free(root_value); 

}

// 랜덤 문자열 생성.
void generate_random_string(char** str)
{
    // random 문자열의 길이는 랜덤으로 정해짐.
    int size = rand()%100+1;// [0~99]+1 = [1~100]

    // 뒤에 NULL 값을 넣어야하므로 +1를 해준다.
    *str = (char*)malloc(size+1);

    for(int i=0;i<size;i++)
    {
       (*str)[i]=charSet[rand()%charSetLen];
    }

    (*str)[size] = '\0';
}

