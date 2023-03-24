#include <stdio.h>


int main() {
    FILE *file;
    unsigned char byte;

    file = fopen("test.txt", "rb");

    if (file == NULL) {
        printf("Error opening file.\n");
        return -1;
    }

    while(fread(&byte, sizeof(unsigned char), 1, file)) {
        printf("%02x ", byte);
    }

    fclose(file);

    return 0;
}