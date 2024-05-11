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
#include <libgen.h> 
#include <stdbool.h>


#define MAX_ARGS 10
#define BUFFER_SIZE 1024

struct Snapshot
{
    char path[256];
    time_t lastModified;
    int size;
    mode_t mode;
    char permissions[11];
    time_t lastAccessed;
};

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


/*

bool performSyntacticAnalysis(const char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Error: Unable to open file");
        return false;
    }

    char line[BUFFER_SIZE];
    const char *delimiters = " \t\n.,;:!?\r(){}[]<>'\"`~";
    bool foundDangerousWord = false;

    while (fgets(line, BUFFER_SIZE, file) != NULL && !foundDangerousWord) {
        char *token = strtok(line, delimiters);
        while (token != NULL) {
            if (strstr(token, "corrupted") || strstr(token, "dangerous") ||
                strstr(token, "risk") || strstr(token, "attack") ||
                strstr(token, "malware") || strstr(token, "malicious")) {
                foundDangerousWord = true;
                break;
            }
            token = strtok(NULL, delimiters);
        }
    }

    fclose(file);
    return foundDangerousWord;
}
*/

void isolateDangerousFile(const char *filePath, const char *outputDir) 
{
    char *fileName = strrchr(filePath, '/');
    if (fileName == NULL) 
    {
        fprintf(stderr, "Error: Invalid file path [%s]\n", filePath);
        return;
    }
    fileName++;

    char destPath[BUFFER_SIZE];
    snprintf(destPath, BUFFER_SIZE, "%s/%s", outputDir, fileName);

    printf("Attempting to move [%s] to [%s]\n", filePath, destPath);

    if (rename(filePath, destPath) == -1) 
    {
        perror("Error: Unable to move file to isolated directory");
        return;
    }

    printf("File successfully isolated: %s\n", destPath);
}



void checkPermissionsAndAnalyze(const char *filePath, const char *safeDir) 
{
    struct stat st;
    if (stat(filePath, &st) == -1) 
    {
        perror("Error: Unable to get file status");
        exit(EXIT_FAILURE);
    }

    // We expect no permissions for user, group, and others (0 permissions)
    if ((st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) == 0)
    {
        printf("No permissions found for file %s, possible tampering detected.\n", filePath);
        

        pid_t pid = fork();
        if (pid == -1) {
            perror("Error: Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) 
        {
            execlp("./verify_for_malicious.sh", "verify_for_malicious.sh", filePath, NULL);
            perror("Error: Failed to execute script");
            exit(EXIT_FAILURE);
        } else { 
            int status;
            waitpid(pid, &status, 0); 
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                printf("Dangerous file detected: %s\n", filePath);
                isolateDangerousFile(filePath, safeDir);
            }
        }
    } else {
        printf("File %s has normal permissions.\n", filePath);
    }
}


void listFilesRecursively( char *dirPath,const char* outputDir,const char *outputDirMalitious)
{
    char path[1000];
    struct dirent *dp;
    struct stat st;
//    struct Snapshot snapshot={0};

    DIR *dir = opendir(dirPath);

    char outputPath[BUFFER_SIZE];
    snprintf(outputPath, BUFFER_SIZE, "%s/%s_metadata.txt", outputDir, basename(dirPath));
    int snapshotFile = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (snapshotFile == -1) 
    {
        perror("Eroare la deschiderea fisierului de iesire");
        exit(EXIT_FAILURE);
    }

    if (!dir)
    {
        perror("argumentul nu e director");
        exit(-1);
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            sprintf(path, "%s/%s", dirPath, dp->d_name);

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
                n = snprintf(buffer, BUFFER_SIZE, "Path: %s\n", path);
                write(snapshotFile, buffer, n);
                n = snprintf(buffer, BUFFER_SIZE, "Size: %ld bytes\n", st.st_size);
                write(snapshotFile, buffer, n);
                n = snprintf(buffer, BUFFER_SIZE, "Mode: %o\n", st.st_mode);
                write(snapshotFile, buffer, n);
                n = snprintf(buffer, BUFFER_SIZE, "Permissions: %s\n", permissions);
                write(snapshotFile, buffer, n);
                n = snprintf(buffer, BUFFER_SIZE, "Last accessed: %s", ctime(&st.st_atime));
                write(snapshotFile, buffer, strlen(buffer));
                n = snprintf(buffer, BUFFER_SIZE, "Last modified: %s", ctime(&st.st_mtime));
                write(snapshotFile, buffer, strlen(buffer));
                write(snapshotFile, "\n", 1);
                //checkAndIsolateDangerousFile(path,outputDirMalitious);
                checkPermissionsAndAnalyze(path,outputDirMalitious);
                //compareAndChangeSnapshots(path, &snapshot, outputFile);
          
            }

            if (S_ISDIR(st.st_mode))
            {
                listFilesRecursively(dirPath,outputDir,outputDirMalitious);
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
            listFilesRecursively(inputDirs[i],outputDir,outputDirMalitious);
            printDirectory(inputDirs[i],0);
            printf("\n");
            exit(EXIT_SUCCESS);
        }

    }

for (int i = 0; i < inputCount; i++)
{
    int status;
    pid_t terminated_pid = wait(&status);
    printf("Captura pentru directorul %d creata cu succes\n",i+1);
    printf("Procesul Copil %d s-a incheiat cu PID %d si cod de iesire %d\n", i+1, terminated_pid, status);
}

    

    return 0;
}