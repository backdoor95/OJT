#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include "pcap_header.h"

#define SNAP_LEN 1518
#define TIME_INTERVAL 60

pcap_t *handle;
FILE *pcap_file;
char errbuf[PCAP_ERRBUF_SIZE];

void close_pcap() {
    if (handle) {
        pcap_close(handle);
    }
    if (pcap_file) {
        fclose(pcap_file);
    }
}

void handle_sigint(int sig) {
    printf("\nSIGINT received, closing pcap and exiting...\n");
    exit(0);
}

void create_new_pcap_file(char *filename) {
    if (pcap_file) {
        fclose(pcap_file);
    }
    pcap_file = fopen(filename, "wb");
    if (pcap_file == NULL) {
        perror("Unable to create pcap file");
        exit(EXIT_FAILURE);
    }

    // Write pcap file header
    file_write_pcap_file_header(pcap_file);
}

void packet_handler(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
    fwrite(header, sizeof(struct pcap_pkthdr), 1, pcap_file);
    fwrite(packet, header->caplen, 1, pcap_file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Register SIGINT handler
    signal(SIGINT, handle_sigint);

    // Clean up when program exits
    atexit(close_pcap);

    // Open the specified network device for packet capture
    handle = pcap_open_live(argv[1], SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", argv[1], errbuf);
        return EXIT_FAILURE;
    }

    time_t start_time = time(NULL);
    char filename[64];
    strftime(filename, sizeof(filename), "%Y_%m_%d_%H_%M.pcap", localtime(&start_time));
    create_new_pcap_file(filename);

    while (1) {
        time_t current_time = time(NULL);
        if (difftime(current_time, start_time) >= TIME_INTERVAL) {
            start_time = current_time;
            strftime(filename, sizeof(filename), "%Y_%m_%d_%H_%M.pcap", localtime(&start_time));
            create_new_pcap_file(filename);
        }

        struct pcap_pkthdr header;
        const u_char *packet = pcap_next(handle, &header);
        if (packet) {
            packet_handler(NULL, &header, packet);
        }
    }

    return 0;
}
