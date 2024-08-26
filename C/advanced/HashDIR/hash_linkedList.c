#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// -------------[설계]-------------
// 해시테이블 = 해시함수 + 버켓배열
// 해시함수 = 해시 코드맵 + 압축맵
// =================================
// 해시 테이블 = 해시함수(해시 코드맵 + 압축맵) + 버켓 배열
// ================================
// 


// =================== 압축맵 ======================
// 문제에서 요구조건은 버켓 배열의 크기를 31개로 세팅할 것. 
// 여기서 왜 31로 세팅을 하라고 한 이유는?
// 이유 
//  1. 해시 충돌 최소화 : 서로 다른 key들이 해시 함수에 의해 동일한 인덱스로 매핑이 된다. 따라서 짝수, 홀수, 제곱수로 해시 테이블의 크기를 설정한다면 해시 충돌이 커진다.
//  2. 모듈러 연산 효과 : 소수로 설정했을 경우가 더 고르게 분포되는 효과
//  3. 해시 함수의 성능 최적화.
//  4. 사이클 회피 : 특정 패턴의 키들이 일정한 간격으로 hash table에 충돌하는 싸이클이 발생할수 있다. 크기가 소수(prime number)라면 이런 사이클을 최소화 할 수 있다.
// 따라서 일반적으로 해시 테이블의 크기 (M)은 소수(prime)로 선택함.
// 결론 : 31이 소수이기 때문.


// =================== 해시 코드맵======================
// 고려사항
//  1. 어떻게 하면 최대한 충돌을 방지할 수 있을까?
//   대안 : 해시 코드맵의 방식중에서 [다항누적 방식(polynomial accumulation)]을 적용하기로 함.
//   이유 : 다항누적 방식이 문자열에 적당한 방식이라고 함. -> 학교 알고리즘 교안 "해시테이블" 강의 자료를 참고할 것.
//   방법 
//      1. 키의 비트들을 고정길이(예: 8, 16, 32 bits)의 요소로 분할 => a(0), a(1), a(2),......, a(n-1)
//      2. 고정값 z를 사용하여 각 요소의 위치에 따른 별도 계산을 부과한 다항식 p(z)를 계산
//          - p(z) = a0 + a1*z + a2*z^2 + a3*z^3 + ..... + a(n-1)*z^(n-1)
//          - 여기서 overflow가 발생할 가능성이 있는데 이걸 고려해야 한다. ==> 알리리즘 교안에서는 오버플로우를 무시하라고 했는데, 적절하게 처리할 수 있도록 코드를 작성할 수 있는지 생각해보기.
//   기대성능
//      - 고정값 z=33을 선택할 경우, 50000개의 영단어에 대해 6회의 충돌 발생
//   학교과제는 key값이 정수였지만 OJT에서는 key값이 문자열이므로 학교과제 코드를 재활용하기에는 무리가 있음. 다시 처음부터 작성할것.
// 


//+++++++++++++++++[Refactoring 했으면 좋을것 같은 부분들]+++++++++++++++++++
// 1. 해시테이블의 적재율(load factor) lf = n/M
//  - 적재율은 낮게 유지되어야함. 가능한 1 아래로
//  - 현재 문제에서 주어진 이미지는 "분리연쇄법"
//  --- lf > 1 : 작동하지만 비효율적
//  --- lf < 1 : (기왕이면 0.75 미만이면), O(1) 기대실행시간 성취가능.
// 2. 재해싱 
//  [1] 해시테이블의 적재율을 상수(0.75)이하로 유지하기 위해서 원소를 삽입할 떄마다. 이 한계를 넘기지 않기 위해 추가작업필요.
//  - when 재해싱?
//  ----- 1) 적재율의 최적치를 초과했을때 
//  ----- 2) 원소 삽입에 실패했을 경우 ---> 예외 처리 필요
//  ----- 3) 너무 많은 비활성 셀들로 포화되어 성능이 저하되었을 때. 
//  [2] 재해싱의 단계
//------- 1) 버켓 배열의 크기를 증가 ( 원래 배열의 2배 크기로, 이때 소수로 설정할것)
//------- 2) 새 크기에 대응하도록 압축맵 수정.
//------- 3) 새 압축맵을 사용하여, 기존 해시테이블의 모든 원소들을 새 테이블에 삽입. 



// 필요한 함수
// Q1. 해시 코드맵
// A1) polynomial accumulation 사용
// Q2. 압축맵
// A2) 1. 나누기 | 2. 승합제
// Q3. 분리연쇄법 알고리즘에 필요한 함수
// A3) 1. 


const int Z = 33;
const int BUCKET_SIZE = 31;

typedef struct linked_list
{
    char* hash_key;
    char* data;
    struct linked_list *next;
}linked_list_t;

typedef struct hash_table
{
    struct linked_list *head;
    int list_entry;
}hash_table_t;

void init_bucket_array(hash_table_t *ht)
{


}

// 해시 함수 = 해시코드맵 + 압축맵

// 해시코드맵 : keys -> integers
int hash_code_map(char* key)
{
    int integers = 0;
    
    // 다항누적
    for(int i = 0; i < strlen(key); i++)
    {
        integers = integers * Z + key[i];
    }

    return integers;
}

// 압축맵 : integers -> [0, BUCKET_SIZE-1] 인덱스화
int compression_map(int integers)
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
    int a = 13;
    int b = 17;

    int index = (a*integers + b)%M;
    return index;
}



int hash_function(char* key)
{
   int integers = hash_code_map(key);
   return compression_map(integers);
}


int main(void)
{

    FILE *file = fopen("hash.csv", "r");
    if(file == NULL)
    {
        error("file 열기 실패\n");
        return exit(1);
    }

   // bucket 배열 size = 31
     
    
    
    
    char line[256];
    while(fgets(line, sizeof(line), file))
    {
        
    }

    



    return 0;
}
    
