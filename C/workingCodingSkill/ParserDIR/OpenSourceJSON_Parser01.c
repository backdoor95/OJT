#include<stdio.h>
#include<time.h>
#include<pthread.h>
#include"./parson.h"



// 1. Json 파일을 읽어서 구조체에 저장
// 2. OpenSourceJSON_Parser01.c에서는 단일 스레드 따라서 멀티스레드 방식을 고려하지 않고 코딩
// 3. Json 파일 요구사항 동작후 Report 구조체에 저장
//      1) repeat_cnt 
//      2) repeat : 랜덤 문자열
//      3) 구조체에 저장
// 4. 구조체에 저장되어 있는 정보를 JSON으로 변환뒤, 파일로 저장& 출력
// 


const char charSet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
int charSetLen = -1;

typedef struct _THREAD
{
	char* name;// random한 문자열의 최대길이는 알파벳 100개까지
}Thread;

typedef struct _JSON_MESSAGE
{
	int repeat_cnt;
	int thread_num;
	Thread* threadList;		
}JsonMessage;

typedef struct _REPORT
{

    int repeat_cnt;
    char**randomStringArray;

}Report;

char* RandString()
{
	// random 문자열의 길이
	int size = rand()%100+1;
    char* Prs = (char*)malloc(size+1);
	
	for(int i=0;i<size;i++)
	{
        Prs[i]=charSet[rand()%charSetLen];

    }
    Prs[size] = '\0';
    return Prs;
}


void ReadJsonAndSaveStructure(JsonMessage *j)
{
    charSetLen = (int)strlen(charSet);
    printf("charSet의 길이는 : %d\n", charSetLen);

    JSON_Value *rootValue;
    JSON_Object *rootObject;

    /* 초기화 */
    rootValue = json_parse_file("jparser.json");      // JSON 파일을 읽어서 파싱 
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
            printf("Error : thread 배열index %zu 오류발생.\n", i);
            continue; // 오류 발생 시 다음 thread로 이동
        }

        const char *name = json_object_get_string(threadObj, "name");
        if (name == NULL) {
            printf("Thread 배열의 인덱스%zu번에서value가 NULL입니다.\n", i);
        } else {
            printf("Thread %zu name: %s\n", i, name);
            // strcpy(dest, src) 함수 사용
            // 검증 필요함. 
            int len = strlen(name);
            j->threadList[i].name = (char*)malloc(sizeof(char)*len+1);
            strcpy(j->threadList[i].name, name);

        }
    }

    // 1. 구조체에 저장
    j->repeat_cnt = repeat_cnt;
    j->thread_num = thread_num;
    j->threadList = (Thread*)malloc(sizeof(Thread)*thread_num);
	
}



int main(void)
{
    srand((unsigned int)time(NULL));
    


    return 0;
}
