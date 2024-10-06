#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcap_header.h"

int file_write_pcap_file_header(void* fp) {
    struct sd_pcap_file_header pcap_hdr;
    FILE *wfp = (FILE *)fp;
    if (!wfp) return -1;

    pcap_hdr.magic = 2712847316 /* A1 B2 C3 D4 */;
    pcap_hdr.version_major = 2;
    pcap_hdr.version_minor = 4;
    pcap_hdr.thiszone = 0;
    pcap_hdr.sigfigs = 0;
    pcap_hdr.snaplen = 65535;
    pcap_hdr.linktype = 1;

    fwrite((const void *)&pcap_hdr, 1, sizeof(struct sd_pcap_file_header), wfp);

    return 0;
}
