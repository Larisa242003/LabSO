#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc,char* argv[])
{
    struct stat st;
    DIR *dir;
    struct dirent *d;

    if(argc!=2)
    {
        perror("eroare folosire argumente");
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
 
    while((d=readdir(dir))!=NULL){
        printf("name: %s", d->d_name);
    }

    return 0;
}