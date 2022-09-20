#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h> 
#include <memory.h>
#include <semaphore.h>

int z;
char txt[100][100];//Store txt's paths
char txt_without_directory[100][100];//Store txt's Name
int txt_counter;//Count how many txt that we found
int word_num;
char** words;
int memory=8;




int readDirectory(char *path)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(path);//Open Directory
    char temp[100];
    strcpy(temp,path);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)//Read Directory
        {   
            int length = strlen(dir->d_name);
            if (strncmp(dir->d_name+ length - 4, ".txt", 4) == 0) 
            {
            strcpy(temp,path);
            strcpy(txt[txt_counter],dir->d_name);
            strcpy(txt_without_directory[txt_counter],dir->d_name);//Copy the name of txt's
            strcat(temp,"/");
            strcat(temp,txt[txt_counter]);
            strcpy(txt[txt_counter],temp);//Create string that store txt's path
            txt_counter++;
            }
        }
        closedir(d);
        return 1;//if we don't get error
    }
    else{
        printf("Wrong Directory\n");//İf path does not exist 
        return 0;//Give a error
    }
}


void* readData(void *param)
{
    int thread_id = pthread_self();//Get thread's İd
    char *path =param;//Get Thread Path
    int i,y,g,repeat,repeat_index;
    char wordByWord[100][100];//Array that store words

    FILE *fp = fopen(path, "r");//Open given path
    if (fp == NULL)
    {
        printf("Error: could not open file %s", path);
    }
    char buffer[256];
    while (fgets(buffer, 256, fp))//Get txt line by line
    {
        for(y=0;y<=strlen(buffer);y++)//Get buffers letter by letter
        {
            if(buffer[y]==' ' && buffer[y+1]!='\n')//if char is space this means this word is over skip to next
            {
                if(strlen(wordByWord[z])>0)
                {
                    z++;//This is skif for next word
                    g=0;//This is for create the word from the zero of array
                }
            }
            else if(buffer[y]!=' ' && buffer[y]!='\n')//if char isn't space and \n this means this is a letter 
            {
                wordByWord[z][g]=buffer[y];//Store the letter to existing word
                g++;//Skip to next letter

            }
            else{
                continue;
            }

        }
        if(strlen(wordByWord[z])>0)
        {
            z++;
            g=0;
        }
        
        
    }
    
        fclose(fp);// close the file
        for(y=0;y<=z;y++)//Search all the words and find if the words showed twice
        {                //If the word appears for the first time, we save it.
            if(strcmp(wordByWord[y],""))
            {
                if(word_num==memory)//Realloc the array
                {
                  memory=memory*2;
                  words = (char*) realloc (words, (memory)*sizeof(char*));
                  printf("Thread %d: Re-allocated array of %d pointers.\n",thread_id, memory);

                }

                repeat=0;
                for (int i = 0; i < word_num; ++i)//Check words showed twice
                {   
                    if (!strcmp(wordByWord[y],words[i]))
                    {
                        repeat_index=i;
                        repeat=1;
                    }
                    
                }
                if (repeat==0)
                {
                    words[word_num] = malloc((strlen(wordByWord[y])+1)*sizeof(char));
                    strcpy(words[word_num],wordByWord[y]);
                    printf("Thread %d: Added %s at index %d. \n",thread_id,words[word_num],word_num);
                    word_num+=1;
                }
                else
                {
                    printf("THREAD %d: The word  \"%s\" has already located at index %d.\n",thread_id,wordByWord[y],repeat_index);
                }
            }
        }
        
    
    return NULL;

}

int main(int argc, char *argv[])
{
    words = malloc((memory)*sizeof(char*));
    z=0;
    if(argc==5)// Yeterli Argüman Belirtilmiş mi
    {
        int y,t,i,conf,array_index = 0, file_num = 0;
        int succes_path;
        int numberOfThreads=atoi(argv[4]);
        char path[100];
        strcpy(path,argv[2]);
        succes_path =readDirectory(path);
        if(txt_counter!=numberOfThreads){//if there is more thread from needed we don't use extras
        numberOfThreads=txt_counter;
        printf("%d\n",numberOfThreads );
        }
        pthread_t *thread;//defininf threads
        thread = malloc(sizeof(pthread_t)*numberOfThreads);
        i=0;
        if(succes_path==1)//if Path exist succes_path==1
        {

            while(i<txt_counter)
            {
                y=0;
                while(y<numberOfThreads)
                {
                conf = pthread_create(&thread[i], NULL, &readData, &txt[i]);//Create threads
                printf("MAIN THREAD: Assigned \"%s\" to worker thread %ld.\n", txt_without_directory[i], thread[i]);
                
                if (conf) 
                {
                    printf("ERROR\n");
                    exit(-1);
                }
                
                y++;
                i++;

                }
                for (i = 0; i < txt_counter; i++) 
                {
                pthread_join(thread[i], NULL);
                }
            }
            printf("MAIN THREAD: All done (successfully read %d words with %d threads from %d files).",z,numberOfThreads,txt_counter);
            exit(EXIT_SUCCESS);
        }
        if(succes_path==0)
        {
            exit(EXIT_SUCCESS);
        }
        }
        
    

    else
    {
        printf("ERROR: Invalid arguments");
    }  
     printf("\n");
    return 0;
    
}