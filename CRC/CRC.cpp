#include<stdio.h>
#include<iostream>
#include<math.h>

using namespace std;

// function to convert integer to binary string
string toBin(long long int num){
    string bin = "";
    if (num == 0)
        return "0";
    while (num){
        if (num&1)
            bin = "1"+bin;
        else
            bin = "0"+bin;
        num = num>>1;
    }
    return bin;
}

// function to convert binary string to decimal
long long int toDec(string bin){
    long long int num = 0;
    for(int i=0; i<bin.length(); i++){
        if (bin.at(i)=='1')
            num += 1<<(bin.length()-i-1);
    }
    return num;
}

// function to compute CRC and codeword
void CRC(string dataword, string generator){
    int l_gen = generator.length();
    long long int gen = toDec(generator);

    long long int dword = toDec(dataword);
    long long int dividend = dword << (l_gen-1);        // append 0s to dividend

    int shft = (int) ceill(log2l(dividend+1)) - l_gen;  // shft specifies the no. of least significant bits not being XORed
    long long int rem;

    while ((dividend >= gen) || (shft >= 0)){
        rem = (dividend >> shft) ^ gen;                 // bitwise XOR the MSBs of dividend with remainder
        // replace the operated MSBs from the dividend with remainder generated
        dividend = (dividend & ((1 << shft) - 1)) | (rem << shft);
        // change shft variable
        shft = (int) ceill(log2l(dividend + 1)) - l_gen;
    }
    // finally, AND the remainder with the initial remainder (=dividend) to get the codeword
    long long int codeword = (dword << (l_gen - 1)) | dividend;
    cout << "Remainder: " << toBin(dividend) << endl;
    cout << "Codeword : " << toBin(codeword) << endl;
}

int main(){
    // Driver code
    string dataword, generator;
    cout << "Dataword: "; cin >> dataword;
    cout << "Generator:"; cin >> generator;
    CRC(dataword, generator);
    return 0;
}
