/*
    CEFET-UnED Petrópolis
    Criptografia - 2023.1
    Autores: Thiago Paes e Eduardo Paes
    Método de Encriptação: AES-128
    Padding: ANSI.X9.23
    Modo de Encriptação: PCBC
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include "aes.h"

const int NUM_BYTES = 16;

uint8_t * xor (uint8_t * x1, uint8_t *x2) {
    uint8_t *xor_array = calloc(NUM_BYTES, sizeof(uint8_t));

    for (int i = 0; i < NUM_BYTES; i++)
        xor_array[i] = x1[i] ^ x2[i];
    return xor_array;
}

uint8_t *pcbc_enc(uint8_t *plain_text, uint8_t *input, uint8_t *key, uint8_t *output)
{
    // Realiza o XOR inicial
    uint8_t *xor_plain_text = xor(plain_text, input);

    // Encripta
    AES128_Encrypt(xor_plain_text, key, output);

    // Gera novo input/IV
    memcpy(input, xor(plain_text, output), NUM_BYTES);
}

void *pcbc_dec(uint8_t *cipher_text, uint8_t *input, uint8_t *key, uint8_t *output)
{
    // Decripta
    AES128_Decrypt(cipher_text, key, output);

    // Realiza o XOR final
    memcpy(output, xor(output, input), NUM_BYTES);

    // Gera novo input/IV
    memcpy(input, xor(output, cipher_text), NUM_BYTES);
}

uint8_t *add_padding(int size_readed, uint8_t *input)
{
    for (int i = size_readed; i < NUM_BYTES - 1; i++)
        input[i] = 0;
    input[NUM_BYTES - 1] = NUM_BYTES - size_readed;
    return input;
}

int minValue(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

int file_exists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{

    // Verifica se o número de argumentos é válido
    if (argc != 5)
    {
        printf("Usage: %s enc|dec input_file output_file\n", argv[0]);
        return 1;
    }

    // Verifica existência do arquivo de saída
    if (file_exists(argv[4]))
    {
        char option;
        printf("File already exists. Do you want to overwrite it? (y/n): ");
        scanf(" %c", &option);
        if (option != 'y' && option != 'Y')
        {
            printf("Exiting...\n");
            return 0;
        }
    }

    // Pega chave de encriptação (16 bytes = 128 bits)
    uint8_t *key = calloc(NUM_BYTES, sizeof(uint8_t));
    char *pass = getpass("Enter passphrase for key: ");

    for (int i = 0; i < minValue(strlen(pass), NUM_BYTES); i++)
    {
        key[i] = pass[i];
    }

    // Define variáveis auxiliares
    uint8_t *iv = calloc(NUM_BYTES, sizeof(uint8_t));
    uint8_t input[NUM_BYTES];
    uint8_t output[NUM_BYTES];
    FILE *file_in;
    FILE *file_out;

    // Informa criptografia
    printf("Starting process:\n  - Encryptation: AES-128\n  - Padding: ANSI.X9.23\n  - Encrypt Mode: PCBC\n\n");

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
        int is_padding_added = 0;
        while ((size_readed = fread(input, 1, NUM_BYTES, file_in)) != 0)
        {
            // Aplica o padding
            if (size_readed != NUM_BYTES)
            {
                add_padding(size_readed, input);
                is_padding_added = 1;
                printf("Padding added (%d).\n", NUM_BYTES - size_readed);
            }

            // Encriptação
            pcbc_enc(input, iv, key, output);

            // Salva no arquivo
            fwrite(output, 1, NUM_BYTES, file_out);
        }

        // Caso o padding não tenha sido adicionado
        if (!is_padding_added)
        {
            // Adiciona o padding manualmente
            memset(input, 0, NUM_BYTES);
            input[15] = NUM_BYTES;

            // Encriptação
            pcbc_enc(input, iv, key, output);

            // Salva no arquivo
            fwrite(output, 1, NUM_BYTES, file_out);

            printf("Padding added (%d).\n", NUM_BYTES);
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
        int size_read = 0, loop_num = 1;
        long loop_limit;

        // Pega o número de loops necessários
        fseek(file_in, 0L, SEEK_END);
        long file_size = ftell(file_in);
        loop_limit = file_size / NUM_BYTES;
        rewind(file_in);

        // Realiza a decriptação
        while ((size_read = fread(input, 1, NUM_BYTES, file_in)) != 0)
        {
            // Decriptação
            pcbc_dec(input, iv, key, output);

            // Remove o padding
            if (loop_num == loop_limit)
            {
                int padding = NUM_BYTES - output[15];
                if (padding > 0)
                {
                    fwrite(output, 1, padding, file_out);
                    printf("Padding removed (%d).\n", output[15]);
                }
                continue;
            }

            // Salva no arquivo
            fwrite(output, 1, NUM_BYTES, file_out);
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