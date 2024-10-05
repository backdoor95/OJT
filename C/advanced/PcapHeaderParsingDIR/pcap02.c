#include <pcap.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h> // getopt 사용을 위해 추가
#include <getopt.h> // GNU getopt_long 사용을 위해 추가

#define ETHER_ADDR_LEN 6

/* 이더넷 헤더 구조체 */
struct ether_header {
    u_char ether_dhost[ETHER_ADDR_LEN]; // 목적지 MAC 주소
    u_char ether_shost[ETHER_ADDR_LEN]; // 소스 MAC 주소
    u_short ether_type;                 // 프로토콜 타입
};

/* IP 헤더 구조체 */
struct ip_header {
    u_char ihl:4;      // 헤더 길이
    u_char version:4;  // 버전
    u_char tos;        // 서비스 타입
    u_short tot_len;   // 전체 길이
    u_short id;        // 식별자
    u_short frag_off;  // 프래그먼트 오프셋
    u_char ttl;        // TTL
    u_char protocol;   // 프로토콜
    u_short check;     // 체크섬
    struct in_addr saddr; // 소스 주소
    struct in_addr daddr; // 목적지 주소
};

/* TCP 헤더 구조체 */
struct tcp_header {
    u_short source;    // 소스 포트
    u_short dest;      // 목적지 포트
    u_int seq;         // 시퀀스 넘버
    u_int ack_seq;     // ACK 넘버
    u_char doff_res;   // 데이터 오프셋과 예약 필드
    u_char flags;      // 플래그
    u_short window;    // 윈도우 사이즈
    u_short check;     // 체크섬
    u_short urg_ptr;   // 긴급 포인터
};

/* 명령줄 옵션 파싱 함수 */
void print_usage(const char *prog_name) {
    printf("사용법: %s -f <pcap 파일 경로>\n", prog_name);
}

int main(int argc, char **argv) {
    char *filename = NULL;
    int opt;
    int option_index = 0;

    /* 옵션 정의를 위한 구조체 배열 */
    static struct option long_options[] = {
        {"file", required_argument, 0, 'f'},
        {"help", no_argument,       0, 'h'},
        {0,      0,                 0,  0 }
    };

    /* 명령줄 옵션 파싱 */
    while ((opt = getopt_long(argc, argv, "f:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                return 0;
        }
    }

    if (filename == NULL) {
        fprintf(stderr, "PCAP 파일 경로를 지정해야 합니다. '-f' 옵션을 사용하세요.\n");
        print_usage(argv[0]);
        return 1;
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // PCAP 파일 열기
    handle = pcap_open_offline(filename, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "pcap 파일 열기 실패: %s\n", errbuf);
        return 1;
    }

    const u_char *packet;
    struct pcap_pkthdr header;
    int packet_num = 0;

    while ((packet = pcap_next(handle, &header)) != NULL) {
        // 이더넷 헤더 길이
        int ethernet_header_len = sizeof(struct ether_header);

        // 이더넷 헤더 가져오기
        struct ether_header *eth = (struct ether_header *)packet;

        // IPv4 패킷인지 확인
        if (ntohs(eth->ether_type) == 0x0800) { // IPv4
            // IP 헤더 가져오기
            struct ip_header *ip = (struct ip_header *)(packet + ethernet_header_len);

            // IP 헤더 길이 계산
            int ip_header_len = ip->ihl * 4;

            // TCP 패킷인지 확인
            if (ip->protocol == 6) { // TCP
                // TCP 헤더 가져오기
                struct tcp_header *tcp = (struct tcp_header *)(packet + ethernet_header_len + ip_header_len);

                // 클라이언트와 서버 IP 주소 및 포트 번호 추출
                char src_ip[INET_ADDRSTRLEN];
                char dst_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(ip->saddr), src_ip, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &(ip->daddr), dst_ip, INET_ADDRSTRLEN);

                u_short src_port = ntohs(tcp->source);
                u_short dst_port = ntohs(tcp->dest);

                // 출력 형식에 맞게 출력
                printf("# %s:%d -> %s:%d [TCP]\n", src_ip, src_port, dst_ip, dst_port);
            }
        }
    }

    pcap_close(handle);
    return 0;
}

