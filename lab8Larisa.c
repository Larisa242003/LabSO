#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc,char *argv[])
{
    if(argc >6 || argc<2)
    {
        perror("Eroare: Numar invalid de argumente");
        exit(-1);
    }

    for(int i=1;i<argc;i++)
    {
        DIR *dir;
        if((dir=opendir(argv[i]))==NULL)
        {
            perror("Eroare:nu e director\n");
            continue;
        }
        pid_t pid=fork();
        if(pid==-1)
        {
            perror("Eroare fork");
            exit(-1);
        }
        else if(pid==0)
        {
            printf("Listarea directorului %s:\n",argv[i]);
            execl("/bin/ls","ls","-l",argv[i],NULL);
        }
        else
        {
            wait(NULL);
        }
    }


    return 0;
}