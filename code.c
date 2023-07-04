#ifdef _WIN32
#include <dirent_win.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void cloneFiles(const char* sourceDir, const char* destDir) {
    DIR* dir;
    struct dirent* entry;
    char sourcePath[256];
    char destPath[256];
    FILE* sourceFile;
    FILE* destFile;
    char buffer[1024];
    size_t bytesRead;

    dir = opendir(sourceDir);
    if (!dir) {
        printf("Error opening source directory: %s\n", sourceDir);
        return;
    }

    #ifdef _WIN32
    _mkdir(destDir);
    #else
    mkdir(destDir, 0777);
    #endif

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(sourcePath, sizeof(sourcePath), "%s/%s", sourceDir, entry->d_name);
            snprintf(destPath, sizeof(destPath), "%s/%s", destDir, entry->d_name);

            #ifdef _WIN32
            struct _stat st;
            #else
            struct stat st;
            #endif

            if (stat(sourcePath, &st) == 0 && S_ISDIR(st.st_mode)) {
                if (strcmp(entry->d_name, "cloned") == 0) {
                    continue;
                }

                #ifdef _WIN32
                _mkdir(destPath);
                #else
                mkdir(destPath, 0777);
                #endif

                cloneFiles(sourcePath, destPath);
            } else {
                sourceFile = fopen(sourcePath, "rb");
                if (!sourceFile) {
                    printf("Error opening source file: %s\n", sourcePath);
                    continue;
                }

                destFile = fopen(destPath, "wb");
                if (!destFile) {
                    printf("Error opening target file: %s\n", destPath);
                    fclose(sourceFile);
                    continue;
                }

                while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
                    fwrite(buffer, 1, bytesRead, destFile);
                }

                fclose(sourceFile);
                fclose(destFile);

                printf("cloned: %s\n", sourcePath);
            }
        }
    }

    closedir(dir);
}

int main() {
    char currentDir[256];
    if (getcwd(currentDir, sizeof(currentDir)) == NULL) {
        printf("Error getting current directory.\n");
        return 1;
    }

    char destDir[256];
    snprintf(destDir, sizeof(destDir), "%s/cloned", currentDir);

    printf("Starting cloning...\n");

    cloneFiles("/", destDir);

    printf("cloning completed.\n");

    return 0;
}