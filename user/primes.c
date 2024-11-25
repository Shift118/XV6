#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int p_left[2]) __attribute__((noreturn));

void primes(int p_left[2]) {
    int prime;
    int num;

    // Read the first number, which will be the prime.
    if (read(p_left[0], &prime, sizeof(int)) == 0) {
        close(p_left[0]);
        exit(0); // Exit if there are no more numbers to process.
    }

    // Print the prime number.
    printf("prime %d\n", prime);

    // Create a new pipe for the next stage of filtering.
    int p_right[2];
    pipe(p_right);

    if (fork() == 0) {
        // Child process: handle the next prime.
        close(p_right[1]); // Close the write end in the child.
        primes(p_right);   // Recursively continue filtering.
    } else {
        // Parent process: filter numbers and pass them to the next stage.
        close(p_right[0]); // Close the read end of the new pipe.

        // Read numbers, filter out multiples of the prime, and write to the next process.
        while (read(p_left[0], &num, sizeof(int)) > 0) {
            if (num % prime != 0) {
                write(p_right[1], &num, sizeof(int));
            }
        }

        // Close pipes when done.
        close(p_left[0]);
        close(p_right[1]);
        wait(0); // Wait for the child process to finish.
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);

    if (fork() == 0) {
        // Child process starts the filtering chain.
        close(p[1]); // Close the write end of the pipe in the child.
        primes(p);   // Start the filtering process.
    } else {
        // Parent process generates numbers.
        close(p[0]); // Close the read end of the pipe in the parent.

        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(int)); // Write each number into the pipe as a 4-byte int.
        }

        close(p[1]); // Close the write end after sending all numbers.
        wait(0); // Wait for the child process to finish.
        exit(0);
    }
}
