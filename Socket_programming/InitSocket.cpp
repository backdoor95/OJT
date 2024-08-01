#include "Common.h"

int main(int argc, char* argv[])
{
        // 소켓 생성
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);// 3번째 argument가 0일때는 tcp, udp일 경우에 자동으로 세팅이 된다.
        if(sock == INVALID_SOCKET) err_quit("socket()");
        printf("[알림] TCP 소켓 생성 성공\n");

        // 소켓 닫기
        close(sock);
        return 0;
}
