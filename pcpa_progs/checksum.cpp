/* Program to compute and verify checksum */
#include<stdio.h>
#include<stdlib.h>

void readTrash(int bytes, FILE *fp){
    unsigned char trash;
    for(int i=0; i<bytes; i++){
        fread(&trash, 1, 1, fp);
    }
}

unsigned short verifyChksum(unsigned char buff[20]){
    unsigned long sum = 0;
    unsigned short n;
    for(int i=0; i<20; i+=2){
        n = (buff[i]<<8)+buff[i+1];
        sum += n;
        if (sum & 0xFFFF0000){
            //carry occured so wrap around
            sum &= 0xFFFF;
            sum++;
        }
    }
    return ~(sum & 0xFFFF);
}

int main(int argc, char *argv[]){
    FILE *fp;
    fp = fopen(argv[1], "r");
    if(fp==NULL){
        printf("File not found\n");
        return 0;
    }

    //read and discard the irrelevant bytes (54 B in total)
    readTrash(54, fp);

    //read 20 B IPv4 header
    unsigned char iphdr[20];
    fread(iphdr, 20, 1, fp);
    
    unsigned short checksum = (iphdr[10]<<8) + iphdr[11];

    printf("Sender's checksum: %d (0x%04X)\n", checksum, checksum);
    unsigned short verifiedChksum = verifyChksum(iphdr);
    printf("Receiver's checksum: %d (0x%04X)\n", verifiedChksum, verifiedChksum); 
    if (verifiedChksum==0)
        printf("---Checksum Verified---\nNo Errors were Detected in IP Header\n");
    else
        printf("---Non-Zero Checksum---Possible Error Detected in IP Header\n");
    fclose(fp);
    return 0;
}
