/*Program to extract header information from pcap file*/

#include<stdlib.h>
#include<stdio.h>
#include<iostream>

using namespace std;

void readTrash(int bytes, FILE *fp){
    unsigned char trash;
    for(int i=0; i<bytes; i++){
        fread(&trash, 1, 1, fp);
    }
}

void toBin(int n){
    string bin[2];
    string bin_digs[16] = {"0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", 
                        "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111"};
    for(int i=1;i>=0;i--){
        bin[i] = bin_digs[n%16];
        n /= 16;
    }
    cout<<bin[0]<<bin[1];
}


int main(int argc, char *argv[]){
    FILE *fp;
    fp = fopen(argv[1], "r");
    if(fp==NULL){
        printf("File not found\n");
        return 0;
    }
    
    //read and discard the libpcap global(24 B) and packet header(16 B) [total 40 B]
    readTrash(40, fp);
    
    printf("--------\nMAC Info:\n--------\n");
    //Dest and Src MAC Addr
    unsigned char dest_mac[6], src_mac[6];
    fread(dest_mac, 6, 1, fp);
    printf("Dest MAC: ");
    for(int i=0; i<6; i++)
        printf("%02X%c", dest_mac[i], (i!=5)?':':'\n');

    fread(src_mac, 6, 1, fp);
    printf("Src MAC: ");
    for(int i=0; i<6; i++)
        printf("%02X%c", src_mac[i], (i!=5)?':':'\n');
    
    //skipping IP protocol, ver, hlen, 
    readTrash(4, fp);

    //IP header starts
    printf("--------\n--------\nIPv4 HEADER:\n--------\n");
    
    //Total Len
    unsigned char iptotlen[2];
    fread(iptotlen, 2, 1, fp);
    printf("Total Length: %d\n", (iptotlen[0]<<8)+iptotlen[1]);
    
    //Identification
    unsigned char iden[2];
    fread(iden, 2, 1, fp);
    printf("Identification: %d (0x%04X)\n", (iden[0]<<8)+iden[1], (iden[0]<<8)+iden[1]);
    
    //Flags, Fragmentations
    unsigned char frag[2];
    fread(frag, sizeof(frag), 1, fp);
    printf("Flags & Frag. Offset: 0x%04X\n", (frag[0]<<8)+frag[1]);
    
    //Time to Live
    unsigned char ttl;
    fread(&ttl, 1, 1, fp);
    printf("Time to Live: %d\n", ttl);
    
    //Protocol
    unsigned char trans_protcol;
    fread(&trans_protcol, 1, 1, fp);
    printf("Transport Protocol Code: %d\n", trans_protcol);
    
    //IP checksum
    unsigned char ipchksum[2];
    fread(ipchksum, 2, 1, fp);
    printf("IP header Checksum: 0x%04X\n", (ipchksum[0]<<8)+ipchksum[1]);
    
    //Src IP
    unsigned char ipsrc[4];
    fread(ipsrc, 4, 1, fp);
    printf("Source IPv4 Address: ");
    for(int i=0; i<4; i++){
        printf("%d%c", ipsrc[i], (i!=3)?'.':'\n');
    }

    //Dest IP
    unsigned char ipdest[4];
    fread(ipdest, 4, 1, fp);
    printf("Destination IPv4 Address: ");
    for(int i=0; i<4; i++){
        printf("%d%c", ipdest[i], (i!=3)?'.':'\n');
    }

    //TCP header starts
    printf("--------\n--------\nTCP HEADER:\n--------\n");

    //src port
    unsigned char srcport[2];
    fread(srcport, 2, 1, fp);
    printf("Source Port: %d\n", (srcport[0]<<8)+srcport[1]);

    //dest port
    unsigned char destport[2];
    fread(destport, 2, 1, fp);
    printf("Destination Port: %d\n", (destport[0]<<8)+destport[1]);

    //sequ num
    unsigned char seq[4];
    fread(seq, 4, 1, fp);
    printf("Sequence Number: ");
    for(int i=0; i<4; i++){
        toBin(seq[i]); 
        printf((i!=3)?" ":"\n");
    }

    //ack num
    unsigned char ack[4];
    fread(ack, 4, 1, fp);
    printf("Acknowledgement Number: ");
    for(int i=0; i<4; i++){
        toBin(ack[i]); 
        printf((i!=3)?" ":"\n");
    }

    //hlen and flags
    unsigned char hlenflags[2];
    fread(hlenflags, 2, 1, fp);
    printf("Header Length: %d\n", hlenflags[0]/16);
    printf("Flags: 0x%03X\n", ((hlenflags[0]%16)<<8)+hlenflags[1]);

    //window size
    unsigned char winsize[2];
    fread(winsize, 2, 1, fp);
    printf("Window Size: %d\n", (winsize[0]<<8)+winsize[1]);

    //TCP checksum
    unsigned char tcpchksum[2];
    fread(tcpchksum, 2, 1, fp);
    printf("TCP Checksum : 0x%04X\n", (tcpchksum[0]<<8)+tcpchksum[1]);
    
    //urgent ptr
    unsigned char urgent[2];
    fread(urgent, 2, 1, fp);
    printf("Urgent Pointer : %d\n--------\n", (urgent[0]<<8)+urgent[1]);

    //close file pointer
    fclose(fp);
    return 0;
}
