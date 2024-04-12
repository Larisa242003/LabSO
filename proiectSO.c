#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


#define MAX_ARGS 10

void listFilesRecursively(const char *basePath) 
{
    char path[1000];
    struct dirent *dp;
    struct stat st;

    DIR *dir = opendir(basePath);

    if (!dir)
    {
        perror("argumentul nu e director");
        exit(-1);
    }

    while ((dp = readdir(dir)) != NULL) 
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) 
        {
            sprintf(path, "%s/%s", basePath, dp->d_name);

            if (stat(path, &st) == 0) 
            {
                printf("File: %s\n", dp->d_name);
                printf("Path: %s\n", path);
                printf("Size: %ld bytes\n", st.st_size);
                printf("Mode: %o\n", st.st_mode);
                printf("Permissions: %o\n", st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
                printf("Last accessed: %s", ctime(&st.st_atime));
                printf("Last modified: %s", ctime(&st.st_mtime));
                printf("\n");
            }

            if (S_ISDIR(st.st_mode)) 
            {
                listFilesRecursively(path);
            }
        }
    }

    closedir(dir);
}

int main(int argc,char* argv[])
{
    struct stat st;


    if(argc < 2 || argc > MAX_ARGS + 1)
    {
        perror("Eroare: Numar invalid de argumente");
        exit(-1);
    }

    for(int i = 1; i < argc; i++)
    {
        if(stat(argv[i], &st) == -1)
        {
            perror("Eroare la stat");
            printf(": %s\n", argv[i]);
            continue;
        }

        if(!S_ISDIR(st.st_mode))
        {
            printf("DEBUG: Mode for %s is %o\n", argv[i], st.st_mode);
            printf("%s nu este un director, va fi ignorat.\n", argv[i]);
            continue;
        }

        printf("Metadatele pentru directorul: %s\n", argv[i]);
        listFilesRecursively(argv[i]);
    }

    return 0;
}