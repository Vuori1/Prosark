#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Task 1: Get_Input (Parent Process P0)
void get_input(char *input) {
    printf("Task 1 (P0): Enter the test string: ");
    scanf("%[^\n]", input);
    printf("Task 1 (P0): Input received.\n");
}

int main() {
    char test_string[1000];
    get_input(test_string);
    int pipe_fd[2];
    pipe(pipe_fd);
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Task 2: Clean_Input (Child Process C0)
        close(pipe_fd[1]); // Close write end
        printf("Task 2 (C0): Cleaning input...\n");
        char cleaned_string[1000];
        int j = 0;
        for (int i = 0; i < strlen(test_string); i++) {
            if ((test_string[i] >= 'a' && test_string[i] <= 'z')) {
                cleaned_string[j] = test_string[i];
                j++;
            }
        }

        cleaned_string[j] = '\0';
        // Create a shared memory segment in C0
        key_t key = ftok("shared_memory_key", 65);
        int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
        // Attach to the shared memory
        char *shared_memory = (char *)shmat(shmid, (void *)0, 0);
        // Write cleaned string to shared memory
        strcpy(shared_memory, cleaned_string);
        shmdt(shared_memory);
        close(pipe_fd[0]); // Close read end
        printf("Task 2 (C0): Cleaned string written to shared memory.\n");
        printf("Task 2 (C0): Cleaned input: %s\n", cleaned_string); // Print the cleaned input.
        printf("Task 2 (C0): Cleaning completed. Terminating C0.\n");
        exit(0);

    } else {
        // Task 1: Get_Input (Parent Process P0)
        printf("Task 1 (P0): Parent process ID (P0): %d\n", getpid());
        printf("Task 1 (P0): Forked child process ID (C0): %d\n", pid);
        close(pipe_fd[0]); // Close read end
        write(pipe_fd[1], test_string, strlen(test_string) + 1);
        close(pipe_fd[1]); // Close write end
        printf("Task 1 (P0): Sending the test string to C0...\n");
        // Wait for C0 to complete
        wait(NULL);
        printf("Task 1 (P0): C0 has completed.\n");
        // Task 3: Find_Missing (Different Process P1)
        key_t key = ftok("shared_memory_key", 65);
        int shmid = shmget(key, 1024, 0666);
        // Attach to the shared memory
        char *shared_memory = (char *)shmat(shmid, (void *)0, 0);
        char missing_alphabets[26];
        int index = 0;
        for (char c = 'a'; c <= 'z'; c++) {
            if (strchr(shared_memory, c) == NULL) {
                missing_alphabets[index] = c;
                index++;
            }
        }

        missing_alphabets[index] = '\0';
        printf("Task 3 (P1): Finding missing alphabets...\n");
        printf("Task 3 (P1): Missing alphabets: %s\n", missing_alphabets);
        // Detach and remove the shared memory segment
        shmdt(shared_memory);
        shmctl(shmid, IPC_RMID, NULL);
        printf("Task 3 (P1): Finding completed. Terminating P1.\n");
        // P0 should terminate after P1
        printf("Task 1 (P0): Terminating P0.\n");
        exit(0);
    }

    // P1 and C0 will continue from here, but P0 should have terminated.
}

// Task 2: Clean_Input (Function Definition)
void clean_input(char *input, char *cleaned) {
    int j = 0;
    for (int i = 0; i < strlen(input); i++) {
        if ((input[i] >= 'a' && input[i] <= 'z')) {
            cleaned[j] = input[i];
            j++;
        }

    }

    cleaned[j] = '\0';
}
