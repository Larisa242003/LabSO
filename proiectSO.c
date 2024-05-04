#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define MAX_ARGS 10
#define BUFFER_SIZE 1024

void printDirectory(const char *basePath, int depth) 
{
    DIR *dir;
    struct dirent *dp;
    struct stat st;

    if ((dir = opendir(basePath)) == NULL) 
    {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }

    while ((dp = readdir(dir)) != NULL) 
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) 
        {
            for (int i = 0; i < depth; i++) 
            {
                printf("    ");
            }
            printf("|-- %s\n", dp->d_name);

            char path[1024];
            snprintf(path,BUFFER_SIZE, "%s/%s", basePath, dp->d_name);
            if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) 
            {
                printDirectory(path, depth + 1);
            }
        }
    }

    closedir(dir);
}

void listFilesRecursively(const char *basePath,int outputFile) 
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
                char buffer[BUFFER_SIZE];
                int n;

                char permissions[11];
                permissions[0] = (st.st_mode & S_IRUSR) ? 'r' : '-';
                permissions[1] = (st.st_mode & S_IWUSR) ? 'w' : '-';
                permissions[2] = (st.st_mode & S_IXUSR) ? 'x' : '-';
                permissions[3] = (st.st_mode & S_IRGRP) ? 'r' : '-';
                permissions[4] = (st.st_mode & S_IWGRP) ? 'w' : '-';
                permissions[5] = (st.st_mode & S_IXGRP) ? 'x' : '-';
                permissions[6] = (st.st_mode & S_IROTH) ? 'r' : '-';
                permissions[7] = (st.st_mode & S_IWOTH) ? 'w' : '-';
                permissions[8] = (st.st_mode & S_IXOTH) ? 'x' : '-';
                permissions[9] = '\0';
                
                n = snprintf(buffer, BUFFER_SIZE, "File: %s\n", dp->d_name);
                write(outputFile, buffer, n);

                n = snprintf(buffer, BUFFER_SIZE, "Path: %s\n", path);
                write(outputFile, buffer, n);

                n = snprintf(buffer, BUFFER_SIZE, "Size: %ld bytes\n", st.st_size);
                write(outputFile, buffer, n);

                n = snprintf(buffer, BUFFER_SIZE, "Mode: %o\n", st.st_mode);
                write(outputFile, buffer, n);

                n = snprintf(buffer, BUFFER_SIZE, "Permissions: %s\n", permissions);
                write(outputFile, buffer, n);

                n = snprintf(buffer, BUFFER_SIZE, "Last accessed: %s", ctime(&st.st_atime));
                write(outputFile, buffer, strlen(buffer));

                n = snprintf(buffer, BUFFER_SIZE, "Last modified: %s", ctime(&st.st_mtime));
                write(outputFile, buffer, strlen(buffer));

                write(outputFile, "\n", 1);
            }

            if (S_ISDIR(st.st_mode)) 
            {
                listFilesRecursively(path,outputFile);
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

    int outputFile = open("outputFile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outputFile == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
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

        char header[100];
        snprintf(header, 100, "Metadatele pentru directorul: %s\n", argv[i]);
        write(outputFile, header, strlen(header));
        listFilesRecursively(argv[i], outputFile);
    }

    printDirectory(argv[1],0);

    close(outputFile);

    return 0;
}