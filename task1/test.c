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
#include <unistd.h>

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
    char *pass = getpass("Enter passphrase for key: ");
    for (int i = 0; i < 16; i++) {
        key[i] = pass[i];
    }

    // Pega o comando do usuário
    char* op = argv[1];

    // Define variáveis auxiliares
    uint8_t iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t input[16];
    uint8_t output[16];
    FILE * file_in;
    FILE * file_out;

    // Informa criptografia
    printf("Starting encryption:\n  - Encryptation: AES-128\n  - Padding: ANSI.X9.23\n  - Encrypt Mode: PCBC\n\n");

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
        int size_readed = 0;
        while((size_readed = fread(input, 1, 16, file_in)) != 0) {
            // Aplica o padding
            if (size_readed != 16) {
                for (int i = size_readed; i < 16; i++) {
                    if (i == 15)
                        input[i] = size_readed;
                    else 
                        input[i] = 0;
                }
                printf("Padding added (%d).\n", size_readed);
            }

            // Encriptação
            AES128_Encrypt(input, key, output);

            // Salva no arquivo
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
        int size_read = 0, loop_num = 0, loop_limit;

        // Pega o número de loops necessários
        fseek(file_in, 0L, SEEK_END);
        int file_size = ftell(file_in);
        loop_limit = file_size / 16;
        rewind(file_in);

        // Realiza a decriptação
        while((size_read = fread(input, 1, 16, file_in)) != 0) {
            // Decriptação
            AES128_Decrypt(input, key, output);

            // Remove o padding
            if (loop_num == loop_limit - 1) {
                int k = 16 - output[15];
                uint8_t tmp[16];
                for (int i = 0; i <= k; i++) {
                    tmp[i] = output[i];
                }
                printf("%s\n", output);
                printf("%s\n", tmp);

                fwrite(tmp, 1, k, file_out);
                printf("Padding removed.\n");
                continue;
            }

            // Salva no arquivo
            fwrite(output, 1, 16, file_out);
            loop_num++;
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