#include <stdio.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <string.h>

int is_special_interface(const char *name) {
    // 특수 인터페이스 이름 목록
    const char *special_interfaces[] = {
        "any",
        "bluetooth-monitor",
        "nflog",
        "nfqueue",
        "dbus-system",
        "dbus-session",
        NULL
    };
    for (int i = 0; special_interfaces[i] != NULL; i++) {
        if (strcmp(name, special_interfaces[i]) == 0) {
            return 1; // 특수 인터페이스입니다.
        }
    }
    return 0; // 일반 인터페이스입니다.
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    pcap_if_t *d;

    // 모든 네트워크 디바이스 가져오기
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "pcap_findalldevs 에러: %s\n", errbuf);
        return 1;
    }

    // 디바이스 목록 순회
    for (d = alldevs; d != NULL; d = d->next) {
        // 특수 인터페이스인지 확인
        if (is_special_interface(d->name)) {
            continue; // 특수 인터페이스는 건너뜁니다.
        }

        printf("디바이스 이름: %s\n", d->name);

        // 디바이스의 주소 목록 순회
        pcap_addr_t *a;
        for (a = d->addresses; a != NULL; a = a->next) {
            if (a->addr && a->addr->sa_family == AF_INET) {
                struct sockaddr_in *addr = (struct sockaddr_in *)a->addr;
                struct sockaddr_in *netmask = (struct sockaddr_in *)a->netmask;

                char ip_str[INET_ADDRSTRLEN];
                char netmask_str[INET_ADDRSTRLEN];

                // IP 주소 변환
                if (inet_ntop(AF_INET, &(addr->sin_addr), ip_str, sizeof(ip_str)) == NULL) {
                    perror("inet_ntop");
                    continue;
                }

                // 서브넷 마스크 변환
                if (inet_ntop(AF_INET, &(netmask->sin_addr), netmask_str, sizeof(netmask_str)) == NULL) {
                    perror("inet_ntop");
                    continue;
                }

                printf("  IP 주소: %s\n", ip_str);
                printf("  서브넷 마스크: %s\n", netmask_str);
            }
        }

        printf("\n");
    }

    // 디바이스 목록 메모리 해제
    pcap_freealldevs(alldevs);

    return 0;
}

