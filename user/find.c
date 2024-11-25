#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// Function to search for the target file
void find(char *path, char *target) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;

    if ((fd = open(path, 0)) < 0) {
        printf("find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        printf("find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // Check if the path is a directory
    if (st.type == T_DIR) {
        // Copy the path and set pointer for concatenation
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) continue;

            // Skip "." and ".."
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;

            // Append directory entry name to path
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;

            // If the entry is a file and matches the target name, print it
            if (stat(buf, &st) < 0) {
                printf("find: cannot stat %s\n", buf);
                continue;
            }

            // Match found
            if (strcmp(de.name, target) == 0) {
                printf("%s\n", buf);
            }

            // If entry is a directory, recursively search inside it
            if (st.type == T_DIR) {
                find(buf, target);
            }
        }
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}
