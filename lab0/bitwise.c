#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define BIT(n) (1<<(n))

char *byte2bin(uint8_t n, char *binstr) 
{
    // one element per bit (0/1)
    uint8_t binary[8];

    int i = 0;
    for(i = 0; i < 8; i++) {
        binary[i] = n % 2;
        n = n / 2;
    }

    // printing binary array in reverse order
    for (int j = 7, k= 0; j >= 0; j--, k++)
        sprintf(binstr + k, "%d", binary[j]);
        
	return binstr;
}

void print_usage(char *name) {
	printf("Usage: %s <action> <byte> <bit no>\n", name);
	printf("\tWhere: <action> one of 'h'|'l'|'r'|'s'|'t'\n"
		"\t\t <byte> 8-bit integer in hexadecimal\n"
		"\t\t <bit no> of bit to operate on\n");
}

int main(int argc, char *argv[])
{
	char a;	// action: 'h', 'l', 'r', 's', 't'
	unsigned long n; // value to convert must be smaller than 256
	int bit; // bit to operate on: must be between 0 an 7
    char binary[9]; // array for binary representation of n,
					//  remember that in C strings are terminated with a 0
                
    a = argv[1][0];
    n = strtoul(argv[2],NULL,16);
    bit = atoi(argv[3]);
    binary[8] = '\0';

	// Validate command line arguments
    if(a != 'h' && a != 'l' && a != 'r' && a != 's' && a != 't'){
        return -1;
    }

    if(n > 256){
        return -2;
    }

    if(bit < 0 || bit > 7){
        return -3; 
    }

	// Print to stdout the binary representation of n
	byte2bin(n,binary);
	// Do what the user asked and print the result
    unsigned compared_value = BIT(bit);
    if(a == 'h'){
        if(n & compared_value){
            printf("Bit %d is high\n", bit);
        }
        else{
            printf("Bit %d is low\n", bit);
        }
        return 0;
    }

    if(a == 'l'){
        if(!(n & compared_value)){
            printf("Bit %d is low\n", bit);
        }
        else{
            printf("Bit %d is high\n", bit);
        }
        return 0;
    }

    if(a == 'r'){
        n &= ~compared_value;
    }

    if(a == 's'){
        n = n | compared_value;
    }

    if(a =='t'){
        n ^= compared_value;
    }

    byte2bin(n,binary);
    printf("The final number will be %lu\n", n);	
    return 0;
}

