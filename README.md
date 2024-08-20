# OJT
On Job Training repository

1. CentOs8 Linux
2. Python
3. C


### Compile(컴파일)

**소스코드를 어셈블리어로 변환**
- 단일소스코드 전체를 어셈블리어(기계어와 1:1 대응)로 변환
- objectfile 이 만들어진다. ex)[source.c -> source.o]
- cpu가 연산하기 위해서는 기계어의 명령이 있어야 한다.

### Linking(링킹)
- 다른 파일에 위치한 소스코드를 한군데 엮어서 하나의 실행파일로 만드는 과정
- objectfile(.o)를 통해서 만든 binary 파일
- 링킹 과정에서 소스파일에 있는 함수들이 어디에 있는지 찾는 과정을 진행한다.

### header file
- 파일을 여러개로 나눠야할 경우
---- ex) #include 같은 명령은 전처리기(Preprocessor)라고 부르는데, 이과정은 컴파일 이전에 실행된다.
- 즉, 100% 복사되어 붙여넣어진다.
- <> : 컴파일러에서 기본으로 지원하는 헤더파일
- "" : 사용자가 직접 제작한 헤더파일

### Makefile

