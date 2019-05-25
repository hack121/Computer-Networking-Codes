#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<regex>

using namespace std;

main(){
    system("ifconfig wlo1 > ifconfig.txt");
    system("nmcli device show wlo1 > nmcli.txt");
    char c1, c2;
    FILE* file1 = fopen("ifconfig.txt", "r");
    FILE* file2 = fopen("nmcli.txt", "r");
    string ifconfig = "";
    while(fscanf(file1, "%c", &c1)==1)
        ifconfig += c1;
    smatch m;
    //netmask
    regex Nmask("netmask ([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})");
    regex_search(ifconfig, m, Nmask);
    cout<<"Netmask : "<<m[1]<<endl;

    string nmcli = "";
    while(fscanf(file2, "%c", &c1)==1)
        nmcli += c1;
    //MAC addr
    regex mac("GENERAL.HWADDR:\\s+([A-Z0-9]{2}:[A-Z0-9]{2}:[A-Z0-9]{2}:[A-Z0-9]{2}:[A-Z0-9]{2}:[A-Z0-9]{2})");
    regex_search(nmcli, m, mac);
    cout<<"MAC address : "<<m[1]<<endl;   
    //ipv4
    regex ipv4("IP4\\.ADDRESS\\[1\\]:\\s+([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}/*[0-9]*)");
    regex_search(nmcli, m, ipv4);
    cout<<"IPv4 address : "<<m[1]<<endl;
    //def gateway
    regex gateway("IP4\\.GATEWAY:\\s+([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})");
    regex_search(nmcli, m, gateway);
    cout<<"Default Gateway : "<<m[1]<<endl;   
    //DNS server
    regex dns("IP4\\.DNS\\[[0-9]+\\]:\\s+([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})");
    auto it = sregex_iterator(nmcli.begin(), nmcli.end(), dns);
    auto it_end = sregex_iterator();
    int cnt = 1;
    for (sregex_iterator i=it; i!=it_end; ++i) {
        smatch match = *i;                                                 
        string match_str = match.str();
        regex_search(match_str, m, dns); 
        cout<<"DNS Server "<<cnt<<" : "<<m[1]<<endl;
        cnt++;
    }   
}
