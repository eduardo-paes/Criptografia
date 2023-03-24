/*
Input: operation file_input file_output
Encryptation: AES-256
Padding: ANSI.X9.23
Encrypt Mode: PCBC
*/ 

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *file;
    unsigned char byte;

    // Verifica se o número de argumentos é válido
    if (argc != 4)
    {
        printf("Error: Invalid number of arguments.\n");
        return -1;
    }

    // Pega o nome do arquivo
    char* op = argv[1];
    char* file_input = argv[2];
    char* file_output = argv[3];

    if (strcmp(op, "enc") == 0) 
    {
        // Abre o arquivo informado
        file = fopen(file_input, "rb");

        // Verifica se o arquivo foi aberto
        if (file == NULL) 
        {
            printf("Error opening file.\n");
            return -1;
        }

        // Inicia leitura do arquivo
        while(fread(&byte, sizeof(unsigned char), 1, file)) {
            printf("%02x ", byte);
        }

        // Fecha o arquivo
        fclose(file);
    }
    else if (strcmp(op, "dec") == 0) 
    {

    }
    else 
    {
        printf("Error: Invalid operation.\n");
        return -1;
    }

    return 0;
}