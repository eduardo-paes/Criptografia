#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define uchar unsigned char 
#define SBOX_SIZE 256
#define NUM_BYTES 4

const unsigned char SBOX[SBOX_SIZE] = {41, 157, 143, 131, 222, 227, 111, 16, 81, 29, 204, 182, 77, 142, 212, 165, 152, 72, 162, 193, 132, 207, 49, 63, 154, 247, 134, 45, 119, 55, 252, 236, 8, 93, 85, 48, 36, 180, 87, 113, 175, 116, 14, 213, 88, 106, 195, 135, 253, 27, 231, 203, 192, 100, 20, 164, 217, 189, 80, 82, 90, 127, 199, 149, 25, 170, 245, 206, 183, 103, 220, 233, 109, 173, 94, 210, 158, 229, 221, 226, 120, 13, 21, 79, 235, 24, 248, 118, 26, 174, 140, 125, 156, 12, 196, 23, 130, 230, 3, 58, 169, 96, 108, 244, 102, 249, 215, 237, 62, 148, 114, 123, 214, 65, 84, 159, 179, 105, 40, 167, 64, 54, 9, 201, 19, 97, 52, 22, 32, 53, 70, 98, 139, 68, 59, 163, 15, 202, 47, 228, 6, 151, 251, 181, 243, 5, 188, 209, 224, 57, 250, 240, 95, 178, 17, 208, 110, 197, 60, 255, 71, 239, 91, 66, 128, 186, 112, 30, 28, 211, 242, 37, 46, 69, 115, 107, 177, 234, 83, 38, 161, 76, 171, 61, 75, 194, 43, 99, 35, 67, 101, 122, 50, 89, 144, 184, 198, 33, 126, 136, 73, 216, 141, 92, 219, 176, 150, 11, 185, 138, 145, 155, 146, 168, 51, 172, 160, 39, 225, 78, 200, 10, 133, 74, 2, 190, 232, 86, 147, 223, 104, 137, 191, 218, 44, 121, 34, 117, 246, 31, 241, 187, 1, 0, 18, 7, 166, 153, 205, 42, 129, 56, 254, 238, 4, 124};

// Retorna o menor entre dois números
int minValue(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

// Retorna o iésimo bit de x
unsigned char bit(unsigned char x, unsigned char i) {
    return (x >> (8 - i)) & 1;
}

float ** allocTable(int size){
    float ** newTable = malloc(size * sizeof(float *));
    for(int i = 0; i < size; ++i) newTable[i] = malloc(size * sizeof(float)); 
    return newTable;
}

void deallocTable(float ** table, int size){
    for(int i = 0; i < size; ++i) free(table[i]); 
    free(table);
}

// Obtém password do usuário através da linha de comando
uint8_t* getPasswordFromCLI() {
    uint8_t* key = calloc(NUM_BYTES, sizeof(uint8_t));
    char* pass = getpass("Enter passphrase for key: ");
    for (int i = 0; i < minValue(strlen(pass), NUM_BYTES); i++)
        key[i] = pass[i];
    return key;
}

// Realiza a encryptacação da mensagem de acordo com a chave informada
uint8_t* getEncryptedValue(char* message, uint8_t* key) {
    uint8_t* w = calloc(strlen(message), sizeof(uint8_t));
    uint8_t* c = calloc(strlen(message), sizeof(uint8_t));
    for (int i = 0; i < strlen(message); i++)
    {
        w[i] = message[i] ^ key[i % NUM_BYTES];
        c[i] = SBOX[w[i]];
    }
    return w;
}

// Obtém a mensagem da linha de comando
char* concatArguments(int argc, char* args[]) {
    int size = 0;
    for (int i = 1; i < argc; i++) {
        size += strlen(args[i]);
        size++;
    }
    char* result = calloc(size, sizeof(char));
    for (int i = 1; i < argc; i++) {
        strcat(result, args[i]);
        if (i < argc - 1)
            strcat(result, " ");
    }
    return result;
}

// Conta o número de bits setados em um número
uint8_t countSetBits(uint8_t n) {
	uint8_t count = 0;
	while (n) {
		count += n & 1;
		n >>= 1;
	}
	return count;
}

// Gera a tabela de bias
void generateBiasTable(float **biasTable) {
    uchar bitValues[SBOX_SIZE][8];
    for (int i = 0; i < SBOX_SIZE; i++)
        for (int j = 0; j < 8; j++)
            bitValues[i][j] = bit(i, j + 1);

    int matches, a, b;
    for (int i = 0; i < SBOX_SIZE; i++) {
        for (int j = 0; j < SBOX_SIZE; j++) {
            matches = 0;
            for (int k = 0; k < SBOX_SIZE; k++) {
                a = 0;
                b = 0;
                for (int l = 0; l < 8; l++) {
                    a ^= bitValues[k][l] & bitValues[i][l];
                    b ^= bitValues[SBOX[k]][l] & bitValues[j][l];
                }
                if (a == b) matches++;
            }
            biasTable[i][j] = matches - SBOX_SIZE / 2;
        }
    }
}

void printBiasTable(float **biasTable) {
    FILE *file_out;
    file_out = fopen("bias_table.txt", "w");
    fprintf(file_out, "Tabela de Bias:\n");
    fprintf(file_out, "===============\n\n");
    for (int i = 0; i < SBOX_SIZE; i++) {
        for (int j = 0; j < SBOX_SIZE; j++) {
            fprintf(file_out, "|%4.0f\t", biasTable[i][j]);
        }
        fputs("|\n", file_out);
    }
    fclose(file_out);
}

int main (int argc, char* argv[]) {

    float ** biasTable = allocTable(SBOX_SIZE);
    generateBiasTable(biasTable);
    printBiasTable(biasTable);
    deallocTable(biasTable, SBOX_SIZE);

    // // Verifica se o número de argumentos é válido
    // if (argc < 2)
    // {
    //     printf("Message is required for encryptation.\n");
    //     return 1;
    // }

    // // Obtém a mensagem a ser encriptada
    // char* message = concatArguments(argc, argv);

    // // Pega chave de encriptação
    // uint8_t* key = getPasswordFromCLI();

    // // Realiza a encryptação da mensagem
    // uint8_t* c = getEncryptedValue(argv[1], key);

    // // Imprime a mensagem encriptada
    // printf("Encrypted message: %s\n", c);    

    return 0;
}