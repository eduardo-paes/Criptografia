#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "aes.h"

int main() {
    uint8_t key[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };    // 16 bytes = 128 bits
    uint8_t input[16];
    uint8_t output[16];
    uint8_t dec[16]; 
    
    printf("Enter with message: ");
    fgets(input, 16, stdin );
        
    printf("Enter passphrase for key: ");
    fgets(key, 16, stdin );
    
    AES128_Encrypt(input, key, output);
    AES128_Decrypt(output, key, dec);
        
    printf("Encrypted message: \n");
    for( int i = 0; i < 16; i++ ) {
        printf("%.2X", output[i] );
    }
    printf("\n");

    printf("Decrypted message: \n");
    printf("%s\n", dec);
    
    return 0;
}
