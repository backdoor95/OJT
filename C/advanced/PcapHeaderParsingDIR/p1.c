#include<stdio.h>
#include<stdlib.h>
#include<pcap.h>

int main(void)
{

    char* dev = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    dev = pcap_findalldevs(errbuf);
    if(dev == NULL)
    {
        printf("장치를 자동으로 찾을 수 없습니다.\n");
    }
    printf("장치 이름 : %s \n", dev);
    return 0;
}
