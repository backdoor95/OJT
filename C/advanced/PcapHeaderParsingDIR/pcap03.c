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
    u_char doff:4;     // 데이터 오프셋
    u_char reserved:4; // 예약됨
    u_char flags;      // 플래그
    u_short window;    // 윈도우 사이즈
    u_short check;     // 체크섬
    u_short urg_ptr;   // 긴급 포인터
};

/* UDP 헤더 구조체 */
struct udp_header {
    u_short source; // 소스 포트
    u_short dest;   // 목적지 포트
    u_short len;    // 길이
    u_short check;  // 체크섬
};

/* 이더넷 헤더 출력 함수 */
void print_ethernet_header(const u_char *buffer) {
    struct ether_header *eth = (struct ether_header *)buffer;

    printf("=== 이더넷 헤더 ===\n");
    printf("소스 MAC 주소: ");
    for (int i = 0; i < ETHER_ADDR_LEN; i++)
        printf("%02x%s", eth->ether_shost[i], (i < ETHER_ADDR_LEN - 1) ? ":" : "");
    printf("\n");

    printf("목적지 MAC 주소: ");
    for (int i = 0; i < ETHER_ADDR_LEN; i++)
        printf("%02x%s", eth->ether_dhost[i], (i < ETHER_ADDR_LEN - 1) ? ":" : "");
    printf("\n");

    printf("프로토콜 타입: 0x%04x\n", ntohs(eth->ether_type));
}

/* IP 헤더 출력 함수 */
void print_ip_header(const u_char *buffer) {
    struct ip_header *ip = (struct ip_header *)(buffer);

    printf("\n=== IP 헤더 ===\n");
    printf("버전: %d\n", ip->version);
    printf("헤더 길이: %d\n", ip->ihl * 4);
    printf("서비스 타입: %d\n", ip->tos);
    printf("전체 길이: %d\n", ntohs(ip->tot_len));
    printf("식별자: %d\n", ntohs(ip->id));
    printf("TTL: %d\n", ip->ttl);
    printf("프로토콜: %d\n", ip->protocol);
    printf("체크섬: %d\n", ntohs(ip->check));

    char src_ip[INET_ADDRSTRLEN];
    char dst_ip[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(ip->saddr), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip->daddr), dst_ip, INET_ADDRSTRLEN);

    printf("소스 IP 주소: %s\n", src_ip);
    printf("목적지 IP 주소: %s\n", dst_ip);
}

/* TCP 헤더 출력 함수 */
void print_tcp_header(const u_char *buffer) {
    struct tcp_header *tcp = (struct tcp_header *)buffer;

    printf("\n=== TCP 헤더 ===\n");
    printf("소스 포트: %d\n", ntohs(tcp->source));
    printf("목적지 포트: %d\n", ntohs(tcp->dest));
    printf("시퀀스 넘버: %u\n", ntohl(tcp->seq));
    printf("ACK 넘버: %u\n", ntohl(tcp->ack_seq));
    printf("데이터 오프셋: %d\n", tcp->doff * 4);
    printf("플래그: 0x%02x\n", tcp->flags);
    printf("윈도우 사이즈: %d\n", ntohs(tcp->window));
    printf("체크섬: %d\n", ntohs(tcp->check));
    printf("긴급 포인터: %d\n", ntohs(tcp->urg_ptr));
}

/* UDP 헤더 출력 함수 */
void print_udp_header(const u_char *buffer) {
    struct udp_header *udp = (struct udp_header *)buffer;

    printf("\n=== UDP 헤더 ===\n");
    printf("소스 포트: %d\n", ntohs(udp->source));
    printf("목적지 포트: %d\n", ntohs(udp->dest));
    printf("길이: %d\n", ntohs(udp->len));
    printf("체크섬: %d\n", ntohs(udp->check));
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
                printf("사용법: %s -f <pcap 파일 경로>\n", argv[0]);
                return 0;
        }
    }

    if (filename == NULL) {
        fprintf(stderr, "PCAP 파일 경로를 지정해야 합니다. '-f' 옵션을 사용하세요.\n");
        fprintf(stderr, "사용법: %s -f <pcap 파일 경로>\n", argv[0]);
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
        printf("\n=== 패킷 #%d ===\n", ++packet_num);
        printf("캡처된 패킷 길이: %d bytes\n", header.caplen);
        printf("실제 패킷 길이: %d bytes\n", header.len);

        // 이더넷 헤더 파싱
        print_ethernet_header(packet);

        // 이더넷 헤더 길이
        int ethernet_header_len = sizeof(struct ether_header);

        // IP 패킷인지 확인
        struct ether_header *eth = (struct ether_header *)packet;
        if (ntohs(eth->ether_type) == 0x0800) { // IPv4
            // IP 헤더 파싱
            struct ip_header *ip = (struct ip_header *)(packet + ethernet_header_len);
            print_ip_header((u_char *)ip);

            // IP 헤더 길이
            int ip_header_len = ip->ihl * 4;

            // TCP 또는 UDP 확인
            if (ip->protocol == 6) { // TCP
                struct tcp_header *tcp = (struct tcp_header *)(packet + ethernet_header_len + ip_header_len);
                print_tcp_header((u_char *)tcp);
            } else if (ip->protocol == 17) { // UDP
                struct udp_header *udp = (struct udp_header *)(packet + ethernet_header_len + ip_header_len);
                print_udp_header((u_char *)udp);
            } else {
                printf("\n알 수 없는 L4 프로토콜: %d\n", ip->protocol);
            }
        } else {
            printf("\n알 수 없는 L3 프로토콜: 0x%04x\n", ntohs(eth->ether_type));
        }
    }

    pcap_close(handle);
    return 0;
}

