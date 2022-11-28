#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

#define OK 0
#define ERROR_CODE -1

void formatternPrinter(char* buf,int lenstr){
    printf(">");
    int size = (int)sizeof(buf);
    for (int i = 0; i < lenstr; i++){
        if (buf[i] != '\n'){
            char ch = buf[i];
            printf("%c",ch);
        }
        else {
            printf("%c",' ');
        }
    }
    printf("<\n");
}

// convert character array to integer
int char2int (const char *array, size_t n)
{
    int nr = 0 , mult = 1;

    while (n--)
    {
        nr += (array[n] - '0') * mult;
        mult *= 10;
    }
    return nr;
}


    int getFrags(const char *src_filename, const char* a, const char* b) {

    FILE* fp = fopen(src_filename, "r");

    if (fp == NULL){
        printf("error: cannot open %s\n", src_filename);
        exit(2);
    }

    /* moving pointer to end of stream */
    if (fseek(fp, 0, SEEK_END) == ERROR_CODE) {
        perror("[ERROR] Cannot fseek() to the file's end");
        return -1;
    }

    /* get total size of the stream (in bytes) */
    long size;
    if ((size = ftell(fp)) == ERROR_CODE) {
        perror("[ERROR] Cannot ftell()");
        return -1;
    }
    rewind(fp); /* move pointer back to beginning of stream */
    srand(0);
    int fragsread = 0;

    /* passing the chars to int */
    int n = char2int(a, strlen(a));
    int m = char2int(b, strlen(b));

    if (n < 0 || m < 0) printf("Type numbers non-negative please\n");

    char* buf = (char*) malloc((m+1) * sizeof(char));

    while (fragsread < n){
        /* getting the starting byte, module is used to prevent cases
 * where the starting byte chosen is closer to the EOF than n bytes */
        int starting_byte = rand() % (size - m + 1);

        fseek(fp,starting_byte,SEEK_SET);
        fread(buf,sizeof *buf,m,fp);
        buf[m] = '\0';

        /* remove all the newline chars from the string and print it; moves pointer to begin of stream and resets string */
        formatternPrinter(buf,m);
        rewind(fp);
        fragsread++;
        buf[0] = '\0';
    }
    free(buf);
    fclose(fp);;
    return OK;
}


int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("usage: cat file numberfrags maxfragsize\n");
        exit(1);
    }

    getFrags(argv[1], argv[2], argv[3]);

    return 0;
}