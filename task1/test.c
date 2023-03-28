/*
    CEFET-UnED Petrópolis
    Criptografia - 2023.1
    Autores: Thiago Paes e Eduardo Paes
    Método de Encriptação: AES-256
    Padding: ANSI.X9.23
    Modo de Encriptação: PCBC
*/ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "aes.h"

const int NUM_BITS = 16;

uint8_t* xor(uint8_t* x1, uint8_t* x2) {
    static uint8_t xor_array[NUM_BITS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < NUM_BITS; i++) {
        xor_array[i] = x1[i] ^ x2[i];
    }
    return xor_array;
}

uint8_t* pcbc(uint8_t* plain_text, uint8_t* input) {
    return xor(plain_text, input);
}

int main(int argc, char *argv[]) {

    // Verifica se o número de argumentos é válido
    if (argc != 5)
    {
        printf("Error: Invalid number of arguments.\n");
        return -1;
    }

    // Pega chave de encriptação (16 bytes = 128 bits)
    uint8_t key[NUM_BITS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    char *pass = getpass("Enter passphrase for key: ");
    for (int i = 0; i < NUM_BITS; i++) {
        key[i] = pass[i];
    }

    // Define variáveis auxiliares
    uint8_t iv[NUM_BITS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t input[NUM_BITS];
    uint8_t output[NUM_BITS];
    FILE * file_in;
    FILE * file_out;

    // Informa criptografia
    printf("Starting encryption:\n  - Encryptation: AES-128\n  - Padding: ANSI.X9.23\n  - Encrypt Mode: PCBC\n\n");

    // Encriptação
    if (strcmp(argv[1], "enc") == 0) 
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
        int padding_added = 0;
        while((size_readed = fread(input, 1, NUM_BITS, file_in)) != 0) {
            // Aplica o padding
            if (size_readed != NUM_BITS) {
                for (int i = size_readed; i < NUM_BITS; i++) {
                    if (i == 15)
                        input[i] = size_readed;
                    else 
                        input[i] = 0;
                }
                padding_added = 1;
                printf("Padding added (%d).\n", size_readed);
            }

            // Encriptação
            AES128_Encrypt(input, key, output);

            // Salva no arquivo
            fwrite(output, 1, NUM_BITS, file_out);
        }

        // Caso o padding não tenha sido adicionado
        if (!padding_added) {
            // Adiciona o padding manualmente
            memset(input, 0, NUM_BITS);
            input[15] = NUM_BITS;

            // Encriptação
            AES128_Encrypt(input, key, output);

            // Salva no arquivo
            fwrite(output, 1, NUM_BITS, file_out);

            printf("Padding added (%d).\n", NUM_BITS);
        }

        // Fecha o arquivo
        fclose(file_in);
        fclose(file_out);

        // Informa o resultado
        printf("Encryption concluded.\n");
    }

    // Decriptação
    else if (strcmp(argv[1], "dec") == 0) 
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
        loop_limit = file_size / NUM_BITS;
        rewind(file_in);

        // Realiza a decriptação
        while((size_read = fread(input, 1, NUM_BITS, file_in)) != 0) {
            // Decriptação
            AES128_Decrypt(input, key, output);

            // Remove o padding------
            if (loop_num == loop_limit - 1) {
                int k = NUM_BITS - output[15];

                if (k > 0) {
                    fwrite(output, 1, NUM_BITS-k, file_out);
                    printf("Padding removed (%d).\n", NUM_BITS-k);
                }
                continue;
            }

            // Salva no arquivo
            fwrite(output, 1, NUM_BITS, file_out);
            loop_num++;
        }

        // Fecha o arquivo
        fclose(file_in);
        fclose(file_out);

        // Informa o resultado
        printf("Decryption concluded.\n");
    }

    // Operação inválida
    else 
    {
        printf("Error: Invalid operation, use [enc|dec].\n");
        return -1;
    }

    return 0;
}