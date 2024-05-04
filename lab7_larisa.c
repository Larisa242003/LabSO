
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>


typedef struct letter_freq{
    char litera;
    int frecventa;
}letter_freq;

void calculeaza_freq(char *cale_fisier,letter_freq *LetterF)
{
    int f=open(cale_fisier,O_RDONLY);
    if(f==-1)
    {
        perror("eroare deschidere fisier");
        exit(-1);
    }

    char buffer[4096];
    int bytes_read;
    while((bytes_read=read(f,buffer,sizeof(buffer)))>0)
    {
        for(int i=0;i<bytes_read;i++)
        {
            char c=buffer[i];
            if(islower(c))
            {
                LetterF[c-'a'].frecventa++;
            }
        }
    }
}

letter_freq letFreq[26]={{'a',0},{'b',0},{'c',0},{'d',0},{'e',0},{'f',0},{'g',0},{'g',0},{'h',0},{'i',0},{'j',0},{'k',0},{'l',0},{'m',0},{'n',0},{'o',0},{'p',0},{'q',0},{'r',0},{'s',0},{'t',0},{'u',0},{'v',0},{'w',0},{'x',0},{'y',0},{'z',0}};

void parcurge_director(char* caleDir,char *fisier_histograma,char* fisier_statistica)
{
    DIR *dir=opendir(caleDir);
    if(dir==NULL)
    {
        perror("eroare deschidere director");
        exit(-1);
    }

    int histograma=open(fisier_histograma,O_WRONLY);
    if(histograma==-1)
    {
        perror("eroare deschidere fisier");
        exit(-1);
    }
    int statistica=open(fisier_statistica,O_WRONLY);
    if(statistica==-1)
    {
        perror("eroare deschidere fisier");
        exit(-1);
    }

    char path[1000];
    struct dirent *dp;
    struct stat st;

     while ((dp = readdir(dir)) != NULL) 
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) 
        {
            sprintf(path, "%s/%s", caleDir, dp->d_name);

            if (stat(path, &st) == 0) 
            {
                calculeaza_freq(path,letFreq);
            }

        }
    }

    for(int i=0;i<26;i++)
    {
        write(fisier_histograma,"%c:%d\n",letFreq[i].litera,letFreq[i].frecventa);
    }


}



int main(int argc,char* argv[])
{
    if(argc!=4)
    {
        perror("eroare folosire argumente");
        exit(-1);
    }

    //char *director=argv[1];
    //char *fisier_histograma=argv[2];
    //char *fisier_statistica=argv[3];

    return 0;
}