/* Program to capture and analyse packets using libpcap */

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h> 
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

void toHex(int n){
    char hex[2];
    char hex_digs[16] = {'0', '1', '2', '3', '4', '5', '6', '7', 
                        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    for(int i=1;i>=0;i--){
        hex[i] = hex_digs[n%16];
        n /= 16;
    }
    printf("%c%c", hex[0], hex[1]);
} 

int main(){
    char *dev; 
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    const u_char *packet;
    struct pcap_pkthdr hdr;     /* pcap.h */

    dev = pcap_lookupdev(errbuf);
    if(dev == NULL){
        printf("%s\n",errbuf);
        return 0;
    }
    printf("Device Interface : %s\n",dev);

    descr = pcap_open_live(dev,BUFSIZ,0,-1,errbuf);
    if(descr == NULL){
        printf("pcap_open_live() : %s\n",errbuf);
        return 0;
    }

    capture:
    packet = pcap_next(descr,&hdr);
    while(packet == NULL)
        packet = pcap_next(descr,&hdr);

    const struct ether_header* eptr;
    const struct ip* ipHeader;
    const struct tcphdr* tcpHeader;
    char sourceIp[INET_ADDRSTRLEN];
    char destIp[INET_ADDRSTRLEN];
    u_int sourcePort, destPort;
    u_char *data;
    int dataLength = 0;

    eptr = (struct ether_header*)packet;
    
    if (ntohs(eptr->ether_type) == ETHERTYPE_IP){
        ipHeader = (struct ip*)(packet + sizeof(struct ether_header));
        inet_ntop(AF_INET, &(ipHeader->ip_src), sourceIp, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ipHeader->ip_dst), destIp, INET_ADDRSTRLEN);

        if (ipHeader->ip_p == IPPROTO_TCP) {
            printf("Grabbed packet of length %d\n",hdr.len);
            printf("Ethernet address length is %d\n",ETHER_HDR_LEN);
            u_char *ptr;
            ptr = (u_char *)eptr->ether_dhost;
            int i = ETHER_ADDR_LEN;
            printf("Destination MAC Address : ");
            do{
                printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
            }while(--i>0);
            printf("\n");

            ptr = (u_char *)eptr->ether_shost;
            i = ETHER_ADDR_LEN;
            printf("Source MAC Address : ");
            do{
                printf("%s%x",(i == ETHER_ADDR_LEN) ? " " : ":",*ptr++);
            }while(--i>0);
            printf("\n");

            tcpHeader = (tcphdr*)(packet + sizeof(struct ether_header) + sizeof(struct ip));
            sourcePort = ntohs(tcpHeader->source);
            destPort = ntohs(tcpHeader->dest);
            data = (u_char*)(packet + sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));
            dataLength = hdr.len - (sizeof(struct ether_header) + sizeof(struct ip) + sizeof(struct tcphdr));

            // convert non-printable characters, other than carriage return, line feed,
            // or tab into periods when displayed.
            printf("Data (in Hex): ");
            for (int i = 0; i < dataLength; i++){
                toHex(data[i]); printf(" ");
            }

            // print the results
            printf("\nSource IP : %s\n", sourceIp);
            printf("Source Port : %d\n", sourcePort);
            printf("Destination IP : %s\n", destIp);
            printf("Destination Port : %d\n",  destPort);
        }else {
           goto capture;
        }
    }else {
        goto capture;
    }
    return 0;
}
