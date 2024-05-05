#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>


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

void performSyntacticAnalysis(const char *filePath) 
{

    FILE *file = fopen(filePath, "r");
    if (file == NULL) 
    {
        perror("Error: Unable to open file");
        exit(EXIT_FAILURE);
    }

    char line[BUFFER_SIZE];
    int lineCount = 0;
    int wordCount = 0;
    int charCount = 0;
    bool foundDangerousWord = false;

    while (fgets(line, BUFFER_SIZE, file) != NULL) 
    {
        lineCount++;
        charCount += strlen(line);

        char *token = strtok(line, " \t\n");
        while (token != NULL) 
        {
            wordCount++;
   
            if (strstr(token, "corrupted") || strstr(token, "dangerous") || strstr(token, "risk") || strstr(token, "attack") ||strstr(token, "malware") || strstr(token, "malicious")) 
            {
                foundDangerousWord = true;
                break;
            }
            token = strtok(NULL, " \t\n");
        }

        if (foundDangerousWord) 
        {
            break;
        }
    }

    fclose(file);

    printf("File: %s\n", filePath);
    printf("Number of lines: %d\n", lineCount);
    printf("Number of words: %d\n", wordCount);
    printf("Number of characters: %d\n", charCount);

    if (foundDangerousWord) 
    {
        printf("Warning: Potentially dangerous content found!\n");
    } 
    else 
    {
        printf("No dangerous content found.\n");
    }
}

void isolateDangerousFile(const char *filePath, const char *outputDir) 
{
    char *fileName = strrchr(filePath, '/');
    if (fileName == NULL) 
    {
        fprintf(stderr, "Error: Invalid file path\n");
        return;
    }
    fileName++;

    char destPath[BUFFER_SIZE];
    snprintf(destPath, BUFFER_SIZE, "%s/%s", outputDir, fileName);

    if (rename(filePath, destPath) == -1) 
    {
        perror("Error: Unable to move file to isolated directory");
        exit(EXIT_FAILURE);
    }

    printf("File isolated: %s\n", filePath);
}



void checkAndIsolateDangerousFile(const char *filePath, const char *outputDir) 
{
    struct stat st;
    if (stat(filePath, &st) == -1) 
    {
        perror("Error: Unable to get file status");
        exit(EXIT_FAILURE);
    }

    if ((st.st_mode & S_IRUSR) && (st.st_mode & S_IWUSR) && (st.st_mode & S_IXUSR) &&
        (st.st_mode & S_IRGRP) && (st.st_mode & S_IWGRP) && (st.st_mode & S_IXGRP) &&
        (st.st_mode & S_IROTH) && (st.st_mode & S_IWOTH) && (st.st_mode & S_IXOTH)) 
    {
  
        return;
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error: Fork failed");
        exit(-1);
    } else if (pid == 0) 
    { 
        performSyntacticAnalysis(filePath);
        execlp("./verify_for_malicious.sh", "verify_for_malicious.sh", filePath, NULL);
        exit(-1);
    } 
        
    isolateDangerousFile(filePath, outputDir);
    
}



void listFilesRecursively(const char *basePath,int outputFile,const char *outputDirMalitious) 
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
                int bytesWritten=write(outputFile, buffer, n);
                if (bytesWritten == -1) 
                {
                perror("Eroare la scrierea in fisier");
                close(outputFile);
                exit(EXIT_FAILURE);
                }

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
                listFilesRecursively(path,outputFile,outputDirMalitious);
            }
            else
            {
                checkAndIsolateDangerousFile(path, outputDirMalitious);
            }
        }
    }

    closedir(dir);
}

int main(int argc,char* argv[])
{
    struct stat st;

    char *outputDir = NULL;
    char *outputDirMalitious=NULL;
    char *inputDirs[MAX_ARGS];
    int inputCount = 0;


    if(argc < 5 || argc > MAX_ARGS + 1)
    {
        perror("Eroare: Numar invalid de argumente");
        exit(-1);
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0) 
        {
            if (i + 1 < argc) 
            {
                outputDir = argv[i + 1];
                i++;
            }
            
            else 
            {
                fprintf(stderr, "Eroare: Directorul de iesire lipseste pentru optiunea -o\n");
                exit(EXIT_FAILURE);
            }
        } 
        else if (strcmp(argv[i], "-s") == 0) 
        {
            if (i + 1 < argc) 
            {
                outputDirMalitious = argv[i + 1];
                i++;
            }
            else 
            {
                printf("Eroare: Directorul de iesire al fisierelor corupte lipseste pentru optiunea -s\n");
                exit(-1);
            }
        }
        
        else
        {
            inputDirs[inputCount++] = argv[i];
        }
    }

    if (outputDir == NULL) 
    {
        fprintf(stderr, "Eroare: Directorul de iesire nu este specificat. Utilizati optiunea -o\n");
        exit(-1);
    }

    if (outputDirMalitious == NULL) 
    {
        fprintf(stderr, "Eroare: Directorul de iesire al fisierelor corupte nu este specificat. Utilizati optiunea -s\n");
        exit(EXIT_FAILURE);
    }

    char outputPath[BUFFER_SIZE];
    snprintf(outputPath, BUFFER_SIZE, "%s/outputFile.txt", outputDir);
    int outputFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (outputFile == -1) 
    {
    perror("Eroare la deschiderea fisierului de iesire");
    exit(EXIT_FAILURE);
    }

    for(int i = 0; i < inputCount; i++)
    {
        if(stat(inputDirs[i], &st) == -1)
        {
            perror("Eroare la stat");
            printf(": %s\n", inputDirs[i]);
            continue;
        }

        if(!S_ISDIR(st.st_mode))
        {
            printf("DEBUG: Mode for %s is %o\n", argv[i], st.st_mode);
            printf("%s nu este un director, va fi ignorat.\n", inputDirs[i]);
            continue;
        }

        pid_t pid=fork();
        if (pid == -1) 
        {
            perror("Error: fork failed");
            exit(-1);

        }else if(pid==0)
        {
            char header[100];
            snprintf(header, 100, "Metadatele pentru directorul: %s\n", inputDirs[i]);
            write(outputFile, header, strlen(header));
            listFilesRecursively(inputDirs[i], outputFile,outputDirMalitious);
            exit(-1);
        }
        
    }
    
for (int i = 0; i < inputCount; i++)
{
    int status;
    pid_t terminated_pid = wait(&status);
    printf("Procesul Copil %d s-a încheiat cu PID %d și cod de ieșire %d\n", i+3, terminated_pid, status);
}

    printDirectory(argv[3],0);

    close(outputFile);

    return 0;
}