#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

	int n = atoi(argv[1]);      // Number of processes
    float p = atof(argv[2]);    // Odd for locking processes
    int t = atoi(argv[3]);      // Lock time for processes
    pid_t pid;
    char readFifo[20], writeFifo[20];
    int readFd, writeFd;

    // Creating n fifos
    for (int i = 0; i < n; i++) {
        char fifo[20];
        sprintf(fifo, "%s%i%s%i", "fifo", i + 1, "to", (i+1) % n + 1);
        mkfifo(fifo, 0666);
    }

    int id = 1; // id for parent process
    for (int i = 2; i < n + 1; i++) { // Creating n - 1 child processes

        if ((pid = fork()) == -1) {
            /* error forking */
            perror("fork");
            return EXIT_FAILURE;
	    }

        else if (pid == 0) {
            /* child process */
            id = i; // id for child processes
            break; // we don't want child processes to create more processes
        }
    }

    // DEFINING DIFFERENT RANDOM SEED FOR EACH PROCESS
    srand(id);

    // Dining philosophers problem solution to avoid deadlock
    // IF the id is pair we open the fifo's reading end first and the fifo's writing end afterwards
    // IF the id is even we open the fifo's writing end first and the fifo's reading end afterwards
    if (id % 2 == 0) {

        // Open fifo's reading end
        sprintf(readFifo, "%s%i%s%i", "fifo", ((id + n - 2) % n) + 1, "to", id);
        if ((readFd = open(readFifo, O_RDONLY)) == -1) {
            perror("Open fifo");
            return EXIT_FAILURE;
        }
        
        // Open fifo's writing end
        sprintf(writeFifo, "%s%i%s%i", "fifo", id, "to", id % n + 1);
        if ((writeFd = open(writeFifo, O_WRONLY)) == -1) {
            perror("Open fifo");
            return EXIT_FAILURE;
        }

    } else {

        // Open fifo's writing end
        sprintf(writeFifo, "%s%i%s%i", "fifo", id, "to", id % n + 1);
        if ((writeFd = open(writeFifo, O_WRONLY)) == -1) {
            perror("Open fifo");
            return EXIT_FAILURE;
        }

        // Open fifo's reading end
        sprintf(readFifo, "%s%i%s%i", "fifo", ((id + n - 2) % n) + 1, "to", id);
        if ((readFd = open(readFifo, O_RDONLY)) == -1) {
            perror("Open fifo");
            return EXIT_FAILURE;
        }

    }
    
    if (id == n)
        write(writeFd, "0\0", 1);

    for (;;) {

        char token[100] = "\0";

        // Read from fifo
        if (read(readFd, token, sizeof(token)) == -1) {

            //ERROR
            printf("%i ERROR!\n", id);
            return EXIT_FAILURE;
        }
        
        // Lock?
        if ((float) rand() * 100 / RAND_MAX < p) {
            printf("[p%i] lock on token (val = %s)\n", id, token);
            sleep(t);
            printf("[p%i] unlock token\n", id);
        }

        //Add 1 to token
        sprintf(token, "%i", atoi(token) + 1);

        // Write to fifo
        write(writeFd, token, strlen(token));
        
    }
}