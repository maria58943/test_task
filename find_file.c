#include<stdio.h>
#include<string.h>
#include <pthread.h>
#include<dirent.h>
#include<stdlib.h>
#include"find_file.h"
#define MAX_LENGTH 40
#define NUM_THREADS 8

typedef struct args_str
{
    int id;
    char in_path [MAX_LENGTH * 8];
    char in_required [MAX_LENGTH];
    char out [MAX_LENGTH * 8];
} args_t;

//char path[], char required[], char * result
void * looking_for_file ( void * args)
{
    args_t * arguments = (args_t *) args; 
    char path[MAX_LENGTH * 8];
    strcpy(path, arguments->in_path);
    char * required = arguments->in_required;
    DIR * dir = NULL;
    DIR * check_dir = NULL;
    struct dirent * ptr = NULL;
    char file_name[MAX_LENGTH];
    int res = -1;
    char result[MAX_LENGTH * 8];

    if(((dir = opendir(path)) != NULL))
    {
        while((ptr = readdir(dir)) != NULL)
        {
            if(strcmp(arguments->out, "NULL") != 0)
                return 1;
            strcpy(file_name, ptr->d_name);
            printf("Thread %d. Checking file %s in %s\n", arguments->id, file_name, path);
            if(strcmp(file_name, required) == 0)
                {
                    closedir(dir);
                    strcat(path, file_name);
                    strcpy(arguments->out, path);
                    strcpy(result, path);
                    return 1;
                }
            else
            {
                if(strchr(file_name, '.') == NULL)
                {
                    strcat(arguments->in_path, file_name);
                    strcat(arguments->in_path, "\\");
                    looking_for_file((void *)arguments);
                    if(strcmp(arguments->out, "NULL") != 0)
                    {
                        return 1;
                    }
                    strcpy(arguments->in_path, path);  
                    
                }
            }     
        }
        closedir(dir);
        return 0;
    }
}