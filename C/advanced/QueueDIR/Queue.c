#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include"parson.h"
#include<stdbool.h>
#include<unistd.h>

// 원형 큐를 배열로 구현
// 나중에 링크드 리스트로 원형배열을 구현해보기?
// 근데 어차피 똑같을 것으로 예상됨.
// 이유 1 : 원형 큐는 사이즈가 정해져 있다. 이걸 연결리스트로 구현하나, 배열로 구현하나 비슷할듯
// 이유 2 : 오히려 성능 저하가 일어날 수 있다는 생각이 든다. 포인터 변수로 다음 노드로 이동해야 하므로, 배열처럼 빠른 인덱싱이 불가능해서?

// 설계
// empty que, full que를 비교하기 위해서 배열의 한칸은 남겨둔다.
// 따라서 size를 +1 해준다.
// capa를 입력받고, N에는 capa + 1 값을 넣어준다.

typedef struct json_message
{
    int thread_num;
    char** thread_name_list;
}json_t;


typedef struct Queue
{
    int f;  //front
    int r;  //rear
    char** Q;// 원형배열
    int N; // 원형 배열의 크기를 결정할 변수
}Que;

Que q;
json_t jt;
int N;
bool print_flag = false;

void json_to_structure(json_t* jt);
void full_queue_exception();
void empty_queue_exception();
int is_empty(Que* q, int N);
int is_full(Que* q, int N);
void init_que(Que* q, int N);
int enqueue(Que* q, int N, char* str);
int dequeue(Que* q, int N, int index);
void traverse(Que* q, int N);
void* thread_task(void* arg);
pthread_mutex_t mutex_var = PTHREAD_MUTEX_INITIALIZER;
void thread_func();
int main(void) 
{
    char k;
    int capa;
    char str_buf[101];
    json_to_structure(&jt);

    printf("Que의 길이를 입력해주세요 : ");
    scanf("%d", &capa);
    getchar();
    N = capa + 1;
    
    init_que(&q, N);

	while (true)
	{
        printf("모드 입력[ 입력(i), 출력하고 종료(p)] : \n");
		scanf("%c", &k);
        getchar();

		switch (k)
		{
		case 'i':// input
            printf("문자열 입력(최대 100자리) : "); 
			fgets(str_buf, sizeof(str_buf), stdin);
            str_buf[strcspn(str_buf, "\n")] = '\0';
            if (enqueue(&q, N, str_buf) == 1)// 정상
			{
				printf("Queue에 저장 성공\n");
			}
			else// overflow
			{
				traverse(&q, N);
				return 0;
			}
            break;
        case 'p':
            thread_func();
            free(q.Q);
            printf("프로그램을 종료합니다\n");
            return 0;
            break;
        default:
            printf("올바른 모드가 아닙니다. 다시 입력해주세요\n");
			break;
		}
	}

    return 0;
}

void full_queue_exception()
{
    printf("Queue가 포화상태 입니다. 데이터 저장 불가\n");
}
void empty_queue_exception()
{
    printf("Queue에 데이터가 없습니다.\n");
}

int is_empty(Que* q, int N)
{
    return ((q->r+1) % N == q->f);
}
int is_full(Que* q, int N)
{
    return ((q->r+2) % N == q->f);
}
void init_que(Que* q, int N)
{
    q->Q = (char**)malloc(sizeof(char*) * N);
    for(int i = 0; i < N; i++)
    {
        q->Q[i] = NULL;
    }
    q->f = 1;
    q->r = 0;
}
int enqueue(Que* q, int N, char* str)
{
	if (is_full(q, N))
	{
		full_queue_exception();
		return 2;
	}
	q->r = (q->r + 1) % N;
	q->Q[q->r] = (char*)malloc(sizeof(char)*strlen(str)+1);
    strncpy(q->Q[q->r], str, strlen(str)+1);
	return 1;
}
int dequeue(Que* q, int N, int index)
{
	if (is_empty(q, N))
	{
		empty_queue_exception();
		return -1;
	}
	printf("thread[ %s ] delete queue data : %s\n", jt.thread_name_list[index] ,q->Q[q->f]);
    free(q->Q[q->f]);
	q->Q[q->f] = NULL;
	q->f = (q->f + 1) % N;
	return 1;// 삭제할 값이 있으면 출력후 1 반환.
}
void traverse(Que* q, int N)
{
    printf("Que 순회 출력\n");
    for(int i = q->f; i < N; i++)
    {
        printf(" %s <-  ", q->Q[i]);
    }
    printf("NULL\n\n");
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

void thread_func()
{
    int thread_num = jt.thread_num;

    pthread_t* worker_threads = (pthread_t*)malloc(sizeof(pthread_t) * thread_num);
    for(int i = 0; i < thread_num; i++)
    {
        int* thread_id = (int*)malloc(sizeof(int));
        *thread_id = i;
        if(pthread_create(&worker_threads[i], NULL, thread_task, thread_id))
        {
            printf("worker thread 생성실패\n");
            exit(1);
        }
    }

    for(int i = 0; i < thread_num; i++)
    {
        pthread_join(worker_threads[i], NULL);
    }

    free(worker_threads);
 
}

void* thread_task(void* arg)
{
    int index = *((int*)arg);
    while(1)
    {
        dequeue(&q, N, index);
        sleep(1);
    }
    pthread_mutex_unlock(&mutex_var);
    pthread_exit(0); 
}



