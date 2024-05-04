#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        perror("Eroare: Numar invalid de argumente");
        exit(-1);
    }

    FILE *file=fopen(argv[1],"r");
    if(file==NULL)
    {
        perror("Eroare deschidere fisier");
        exit(-1);
    }

    int pipe_cat_grep[2];
    int pipe_grep_print[2];

    if(pipe(pipe_cat_grep)==-1 || pipe(pipe_grep_print)==-1)
    {
        perror("Eroare creare pipe uri");
        exit(-1);
    }

    pid_t cat_pid=fork();
    if(cat_pid==-1)
    {
        perror("eroare fork");
        exit(-1);
    }

    if(cat_pid==0)
    {
        close(pipe_cat_grep[0]);
        dup2(pipe_cat_grep[1],1);
        execlp("cat","cat",argv[1],NULL);
        
    }
    else 
    {

        close(pipe_cat_grep[1]);
        pid_t grep_pid=fork();
        if(grep_pid==-1)
        {
            perror("eroare fork");
            exit(-1);
        }
        if(grep_pid==0)
        {
            //char buff[4096];
            //read(pipe_cat_grep[0],buff,4096);
            dup2(pipe_cat_grep[0],1);
            close(pipe_cat_grep[0]);
            //dup2(pipe_grep_print[1],1);
            execlp("grep","grep",argv[2],NULL);
            

        }
        char buff[4096];
        read(grep_pid,buff,4096);
        printf("%s\n",buff);



    }
    return 0;
}