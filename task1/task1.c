/*
Input: operation file_input -o file_output
Encryptation: AES-256
Padding: ANSI.X9.23
Encrypt Mode: PCBC
*/ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "aes.h"

int main(int argc, char *argv[]) {

    // Verifica se o número de argumentos é válido
    if (argc != 5)
    {
        printf("Error: Invalid number of arguments.\n");
        return -1;
    }

    // Pega chave de encriptação (16 bytes)
    uint8_t key[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, }; // 16 bytes = 128 bits
    printf("Enter passphrase for key: ");
    fpurge(stdin);
    fgets(key, 16, stdin );

    // Pega o comando do usuário
    char* op = argv[1];

    // Define variáveis auxiliares
    uint8_t input[16];
    uint8_t output[16];
    FILE * file_in;
    FILE * file_out;

    // Informa criptografia
    printf("Starting encryption:\n  - Encryptation: AES-256\n  - Padding: ANSI.X9.23\n  - Encrypt Mode: PCBC\n\n");

    if (strcmp(op, "enc") == 0) 
    {
        // Abre o arquivo informado
        file_in = fopen(argv[2], "rb");
        file_out = fopen(argv[4], "wb");

        // Verifica se o arquivo foi aberto
        if (file_in == NULL) 
        {
            printf("Error opening file.\n");
            return -1;
        }

        // Inicia leitura do arquivo
        int size_read = 0;
        while((size_read = fread(input, 1, 16, file_in)) != 0) {
            // Aplica o padding
            if (size_read != 16) {
                printf("Add padding.\n");
            }

            // printf("%02x ", readed);
            AES128_Encrypt(input, key, output);
            fwrite(output, 1, 16, file_out);
        }

        // Fecha o arquivo
        fclose(file_in);
        fclose(file_out);

        // Informa o resultado
        printf("Encryption concluded.\n");
    }
    else if (strcmp(op, "dec") == 0) 
    {
        // Abre o arquivo informado
        file_in = fopen(argv[2], "rb");
        file_out = fopen(argv[4], "wb");

        // Verifica se o arquivo foi aberto
        if (file_in == NULL) 
        {
            printf("Error opening file.\n");
            return -1;
        }

        // Inicia leitura do arquivo
        int size_read = 0;
        while((size_read = fread(input, 1, 16, file_in)) != 0) {
            // Remove o padding
            if (size_read != 16) {
                printf("Remove padding.\n");
            }

            // printf("%02x ", readed);
            AES128_Decrypt(input, key, output);
            fwrite(output, 1, 16, file_out);
        }

        // Fecha o arquivo
        fclose(file_in);
        fclose(file_out);

        // Informa o resultado
        printf("Decryption concluded.\n");
    }
    else 
    {
        printf("Error: Invalid operation, use [enc|dec].\n");
        return -1;
    }

    return 0;
}