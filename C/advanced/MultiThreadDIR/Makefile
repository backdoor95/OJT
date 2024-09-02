# 컴파일러 설정
CC = gcc
CFLAGS = -Wall -g

# 타겟 이름 설정
TARGET = multi

# 소스 파일 목록
SRCS = multi_thread_TLS.c parson.c

# 오브젝트 파일 목록
OBJS = $(SRCS:.c=.o)

# 기본 타겟
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 개별 소스 파일 컴파일
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 클린업: 오브젝트 파일 및 실행 파일 삭제
clean:
	rm -f $(OBJS) $(TARGET)

# 실행 파일을 지우고 다시 빌드
rebuild: clean all

