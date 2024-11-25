#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main(int argc, char *argv[]){
    if(argc != 1){
        fprintf(2, "Usage: pingpong\n");
        exit(1);
    }
    int p[2];

    pipe(p);
    if(fork() == 0){
        close(p[1]);
        char c;
        read(p[0], &c, 1);
        printf("%d: received ping\n", getpid());
        close(p[0]);
        write(p[1], "c", 1);
        close(p[1]);
    } else {
        close(p[0]);
        write(p[1], "c", 1);
        wait(0);
        close(p[1]);
        char c;
        read(p[0], &c, 1);
        printf("%d: received pong\n", getpid());
        close(p[0]);
    }

        
    
    exit(0);
}