#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
Função auxiliar para tornar o nome de um ficheiro .txt para .epub 
*/
char* desiredFormat(char* oldFormat , int strlen){
    char* res = malloc(sizeof(oldFormat + 2));

    for(int i = 0; i < strlen; i++){
        if(oldFormat[i] != '.')
            res[i] = oldFormat[i];
        else {
            res[i] = '.';
            res[i + 1]= 'e';
            res[i + 2] = 'p';
            res[i + 3] = 'u';
            res[i + 4] = 'b';
            res[i + 5] = 0;
            break;
        }
    }
    return res;

}


int main(int argc, char* argv[]) {
    
    pid_t pid;
    int n = argc - 1;
    char* programName = "pandoc";
    char* arg1 = "-o";

    /*
    array que irá guardar todos os nomes dos ficheiros .epub para posteriormente serem
    zipados
    */
    int id = 0;
    for (int i = 1; i < n + 1; i++) { // Creating n - 1 child processes

        if ((pid = fork()) == -1) {
            /* error forking */
            perror("fork");
            return EXIT_FAILURE;
        }

        else if (pid == 0) {
            /* child process */
            id = i;
            break; // we don't want child processes to create more processes
        }
    }

    if (id != 0){
        printf("[%d] converting %s\n", getpid(), argv[id]);

        char *arr1 = argv[id];
        char *arr2 = desiredFormat(arr1, strlen(arr1));

        //Conversão dos ficheiros .txt em ficheiros .epub
        if(execlp(programName, programName, arr1, "-o", arr2, NULL) == -1){
            perror("Could not execve");
        }
    }    

    while(wait(NULL) > 0);

    char* zipProgram = "zip";
    char* zipName = "./ebooks.zip";
    char *strings[argc + 2];
    strings[0] = zipProgram;
    strings[1] = zipName;
    strings[argc + 1] = 0;
    for (int i = 1; i < argc; i++) {
        strings[1 + i] = desiredFormat(argv[i], strlen(argv[i])); 
    }
    //Uso do execvp para chamar o método necessário para zippar os nosso ficheiros convertidos
    if (execvp(zipProgram, strings) == -1)
        printf("Error creating zip");
}
