#program to generate CRC codeword
from math import log, ceil

def CRC(dataword, generator):
    dword = int(dataword, 2)
    l_gen = len(generator)
    dividend = dword << (l_gen - 1)      # append 0s to dividend
    shft = ceil(log(dividend + 1, 2)) - l_gen     # shft specifies the no. of least significant bits not being XORed
    #ceil(log(dividend+1 , 2)) is the no. of binary digits in dividend
    generator = int(generator, 2)

    while dividend >= generator or shft >= 0:
        rem = (dividend >> shft) ^ generator    # bitwise XOR the MSBs of dividend with remainder
        # replace the operated MSBs from the dividend with remainder generated
        dividend = (dividend & ((1 << shft) - 1)) | (rem << shft)
        # change shft variable
        shft = ceil(log(dividend+1, 2)) - l_gen

    # finally, AND the remainder with the initial remainder (=dividend)
    codeword = dword << (l_gen-1)|dividend
    codeword = bin(codeword).lstrip("-0b")
    rem = bin(dividend).lstrip("-0b")
    print("Remainder:", 0 if rem=="" else rem)
    print("Codeword :", 0 if codeword=="" else codeword)

# Driver code
dataword = input("Dataword:  ")
generator = input("Generator: ")
CRC(dataword, generator)
