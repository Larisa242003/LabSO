#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

void listFilesRecursively(const char *basePath) {
    char path[1000];
    struct dirent *dp;
    struct stat st;

    DIR *dir = opendir(basePath);

    if (!dir)
    {
        perror("argumentul nu e director");
        exit(-1);
    }

    while ((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            sprintf(path, "%s/%s", basePath, dp->d_name);

            if (stat(path, &st) == 0) {
                printf("Path: %s\n", path);
                printf("Size: %ld bytes\n", st.st_size);
                printf("Mode: %o\n", st.st_mode);
                printf("Last accessed: %s", ctime(&st.st_atime));
                printf("Last modified: %s", ctime(&st.st_mtime));
                printf("\n");
            }

            if (S_ISDIR(st.st_mode)) {
                listFilesRecursively(path);
            }
        }
    }

    closedir(dir);
}

int main(int argc,char* argv[])
{
    struct stat st;
    DIR *dir;
    //struct dirent *d;

    if(argc!=2)
    {
        perror("eroare folosire argumente");
        exit(-1);
    }

    if(argc>2)
    {
        perror("prea multe argumente");
        exit(-1);
    }

    if((dir=opendir(argv[1]))==NULL)
    {
        perror("argumentul nu e director");
        exit(-1);
    }

    int status;
    status = stat(argv[1], &st);
    printf("%d", status);

    if (!S_ISDIR(st.st_mode)) 
    {
        fprintf(stderr, "%s nu este un director\n", argv[1]);
        exit(EXIT_FAILURE);
    }
 
    /*while((d=readdir(dir))!=NULL){
        printf("name: %s", d->d_name);
    }*/

    listFilesRecursively(argv[1]);

    return 0;
}