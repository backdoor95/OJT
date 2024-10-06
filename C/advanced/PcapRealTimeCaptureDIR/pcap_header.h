#ifndef __PCAP_HEADER_H__
#define __PCAP_HEADER_H__

struct sd_pcap_file_header {
    u_int32_t magic;
    u_int16_t version_major;
    u_int16_t version_minor;
    u_int32_t thiszone; /* gmt to local correction */
    u_int32_t sigfigs;    /* accuracy of timestamps */
    u_int32_t snaplen;    /* max length saved portion of each pkt */
    u_int32_t linktype;   /* data link type (LINKTYPE_*) */
};

int file_write_pcap_file_header(void* fp);

#endif
