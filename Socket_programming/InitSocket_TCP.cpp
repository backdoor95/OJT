#include "Common.h"

int main(int argc, char* argv[])
{
	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock == INVALID_SOCKET) err_quit("socket()");
	printf("[알림] TCP 소켓 생성 성공\n");

	// 소켓 닫기
	close(sock);
	return 0;
}
