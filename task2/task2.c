#pragma region Global Scope

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#define uchar unsigned char
#define BYTE_RANGE 256
#define SBOX_SIZE 256
#define KEY_NUM_BYTES 4

const uchar SBOX[SBOX_SIZE] = {41, 157, 143, 131, 222, 227, 111, 16, 81, 29, 204, 182, 77, 142, 212, 165, 152, 72, 162, 193, 132, 207, 49, 63, 154, 247, 134, 45, 119, 55, 252, 236, 8, 93, 85, 48, 36, 180, 87, 113, 175, 116, 14, 213, 88, 106, 195, 135, 253, 27, 231, 203, 192, 100, 20, 164, 217, 189, 80, 82, 90, 127, 199, 149, 25, 170, 245, 206, 183, 103, 220, 233, 109, 173, 94, 210, 158, 229, 221, 226, 120, 13, 21, 79, 235, 24, 248, 118, 26, 174, 140, 125, 156, 12, 196, 23, 130, 230, 3, 58, 169, 96, 108, 244, 102, 249, 215, 237, 62, 148, 114, 123, 214, 65, 84, 159, 179, 105, 40, 167, 64, 54, 9, 201, 19, 97, 52, 22, 32, 53, 70, 98, 139, 68, 59, 163, 15, 202, 47, 228, 6, 151, 251, 181, 243, 5, 188, 209, 224, 57, 250, 240, 95, 178, 17, 208, 110, 197, 60, 255, 71, 239, 91, 66, 128, 186, 112, 30, 28, 211, 242, 37, 46, 69, 115, 107, 177, 234, 83, 38, 161, 76, 171, 61, 75, 194, 43, 99, 35, 67, 101, 122, 50, 89, 144, 184, 198, 33, 126, 136, 73, 216, 141, 92, 219, 176, 150, 11, 185, 138, 145, 155, 146, 168, 51, 172, 160, 39, 225, 78, 200, 10, 133, 74, 2, 190, 232, 86, 147, 223, 104, 137, 191, 218, 44, 121, 34, 117, 246, 31, 241, 187, 1, 0, 18, 7, 166, 153, 205, 42, 129, 56, 254, 238, 4, 124};

#pragma endregion

// Retorna o iésimo bit de x
uchar bit(uchar x, uchar i)
{
    return (x >> (8 - i)) & 1;
}

// Aloca a tabela de bias
float **allocTable(int size)
{
    float **newTable = malloc(size * sizeof(float *));
    for (int i = 0; i < size; ++i)
        newTable[i] = malloc(size * sizeof(float));
    return newTable;
}

// Desaloca a tabela de bias
void deallocTable(float **table, int size)
{
    for (int i = 0; i < size; ++i)
        free(table[i]);
    free(table);
}

// Gera a tabela de bias
void generateBiasTable(float **biasTable)
{
    uchar bitValues[SBOX_SIZE][8];
    for (int i = 0; i < SBOX_SIZE; i++)
        for (int j = 0; j < 8; j++)
            bitValues[i][j] = bit(i, j + 1);

    int matches, a, b;
    for (int x = 0; x < SBOX_SIZE; x++)
    {
        for (int y = 0; y < SBOX_SIZE; y++)
        {
            matches = 0;
            for (int X = 0; X < SBOX_SIZE; X++)
            {
                a = 0;
                b = 0;
                for (int i = 0; i < 8; i++)
                {
                    a ^= bitValues[X][i] & bitValues[x][i];
                    b ^= bitValues[SBOX[X]][i] & bitValues[y][i];
                }
                if (a == b)
                {
                    matches++;
                }
            }
            biasTable[x][y] = matches - SBOX_SIZE / 2;
        }
    }

    // Salva no arquivo a tabela de bias
    FILE *fileOut;
    fileOut = fopen("bias_table.txt", "w");
    fprintf(fileOut, "Bias Table:\n");
    fprintf(fileOut, "===============\n\n");
    for (int i = 0; i < SBOX_SIZE; i++)
    {
        for (int j = 0; j < SBOX_SIZE; j++)
        {
            fprintf(fileOut, "|%4.0f\t", biasTable[i][j]);
        }
        fprintf(fileOut, "|\n");
    }
    fclose(fileOut);
}

// Encontra as melhores expressões lineares para a chave e realiza o ataque linear
void linearAttack(float **biasTable, char *m, uint8_t *k)
{
    // Encontra o maior bias e o número de ocorrências
    int maxBias = 0;
    int numMaxBias = 0;
    for (int i = 0; i < SBOX_SIZE; i++)
    {
        for (int j = 0; j < SBOX_SIZE; j++)
        {
            if (i == 0 && j == 0)
                continue;

            if (fabsf(biasTable[i][j]) > maxBias)
            {
                maxBias = fabsf(biasTable[i][j]);
                numMaxBias = 1;
            }
            else if (fabsf(biasTable[i][j]) == maxBias)
            {
                numMaxBias++;
            }
        }
    }

    printf("Best Bias = abs(%d)\n", maxBias);
    printf("Occurrence number = %d\n", numMaxBias);

    // Encontra as posições (X,Y) do maior bias
    int posMaxBias[numMaxBias][2];
    int pos = 0;
    for (int i = 0; i < SBOX_SIZE; i++)
    {
        for (int j = 0; j < SBOX_SIZE; j++)
        {
            if (fabsf(biasTable[i][j]) == maxBias)
            {
                posMaxBias[pos][0] = i;
                posMaxBias[pos][1] = j;
                pos++;
            }
        }
    }

    // Imprime a quantidade de ocorrências
    int *posX = calloc(8, sizeof(int));
    int *posY = calloc(8, sizeof(int));
    int X, Y, y, c = 0, pr = 0;
    uchar res, w, yi, r0, r1;
    uint8_t ki = 0;
    for (int i = 0; i < numMaxBias; i++)
    {
        X = posMaxBias[i][0];
        Y = posMaxBias[i][1];
        for (int j = 1; j <= 8; j++)
        {
            if (bit(X, j) == 1)
                posX[j - 1] = 1;
            if (bit(Y, j) == 1)
                posY[j - 1] = 1;
        }

        y = 0;
        for (int x = 0; x < SBOX_SIZE; x++)
        {
            /// Pegar os bits mais significativos de X e Y
            /// Faz o xor entre eles e incrementa o contador
            res = 0;
            y = SBOX[x];
            for (int j = 1; j <= 8; j++)
            {
                if (posX[j - 1])
                    res ^= bit(x, j);
                if (posY[j - 1])
                    res ^= bit(y, j);
            }
            if (res)
                c++;
        }

        // Calcula a probabilidade
        for (uchar l = 0; l < strlen(m); l++)
        {
            r0 = 0;
            r1 = 0;
            w = m[l];
            ki = k[l % KEY_NUM_BYTES];
            yi = SBOX[w ^ ki];

            for (int j = 1; j <= 8; j++)
            {
                if (posX[j - 1])
                {
                    r0 ^= bit(w, j);
                    r1 ^= bit(ki, j);
                }
                if (posY[j - 1])
                    r1 ^= bit(yi, j);
            }

            // Calcula a probabilidade da chave
            if (r0 == (r1 ^ 1))
                pr++;
        }

        // Reseta array de posições de bit 1 de X e Y
        posX = calloc(8, sizeof(int));
        posY = calloc(8, sizeof(int));

        // Imprime o resultado
        printf("X = %d, Y = %d, c = %d, Pr. = %.3f\t| Pr. AL: %f\n", X, Y, c, c / (float)SBOX_SIZE, pr / (256.));
        c = 0;
        pr = 0;
    }
}

int main(int argc, char *argv[])
{
    uint8_t k[KEY_NUM_BYTES] = {0xA5, 0x12, 0x3C, 0x6B};
    char *m = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";

    // Gera a tabela de bias
    float **biasTable = allocTable(SBOX_SIZE);
    generateBiasTable(biasTable);

    // Obtém melhores expressões lineares para a chave
    linearAttack(biasTable, m, k);

    // Desaloca a tabela de bias
    deallocTable(biasTable, SBOX_SIZE);
    return 0;
}