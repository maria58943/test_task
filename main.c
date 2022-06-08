#include<stdio.h>
#include<string.h>
#include <pthread.h>
#include<dirent.h>
#include"find_file.c"
#include<stdlib.h>

#define MAX_LENGTH 40
#define NUM_THREADS 8


int main()
{
    char start_dir[] = "C:\\";
    char required [MAX_LENGTH]; 
    printf("Enter a name of file you are looking for: ");
    scanf("%s", required);
    
    DIR * dir = NULL;
    struct dirent *ptr = NULL;
    char file_name[256];
    
    args_t start_args;
    pthread_t threads[NUM_THREADS];
    args_t args[NUM_THREADS];
    int streams = 0;//number of threads
    int status;
    int i;
    int status_addr;

    strcpy(start_args.in_path, start_dir);
    strcpy(start_args.in_required, required);
    strcpy(start_args.out, "NULL");

    for(i = 0; i < NUM_THREADS; i++)
    {   
        args[i].id = i + 1;
        strcpy(args[i].in_path, start_dir);
        strcpy(args[i].in_required, required);
        strcpy(args[i].out, "NULL");
    }

    if(((dir = opendir(start_dir)) != NULL))
    {
        while((ptr = readdir(dir)) != NULL)
        {
            strcpy(file_name, ptr->d_name);
            printf("Main. Checking file %s in %s\n", file_name, start_dir);
            if(strcmp(file_name, required) == 0)
                {
                    printf("The file %s has been succesfully found\n", file_name);
                    printf("Directory: %s%s\n", start_dir, required);
                    break;
                }
            else
            {
                if(strchr(file_name, '.') == NULL)
                {
                    if(streams < 8)
                    {
                        strcat(args[streams].in_path, file_name);
                        strcat(args[streams].in_path, "\\");
                        streams++;
                        
                    }
                    else
                    {
                        for(i = 0; i < streams; i++)
                        {
                            status = pthread_create(&threads[i], NULL, looking_for_file, (void*) &args[i]);
                            if (status != 0) 
                            {
                                printf("main error: can't create thread, status = %d\n", status);
                                exit(1);
                            }
                        }
                        for (i = 0; i < streams; i++) 
                        {
                            status = pthread_join(threads[i], (void**)&status_addr);
                            if (status != 0) 
                            {
                                printf("main error: can't join thread, status = %d\n", status);
                                exit(1);
                            }
                            if(strcmp(args[i].out, "NULL") != 0)
                            {
                                streams = 0;
                                strcpy(start_args.out, args[i].out);
                                for(i = 0; i < streams; i++)
                                    strcpy(args[i].out, start_args.out);
                                break;
                            }
                        }
                        if(strcmp(start_args.out, "NULL") != 0)
                        {
                            streams = 0;
                            break;
                        }
                        for(i = 0; i < NUM_THREADS; i++)
                        {   
                            args[i].id = i + 1;
                            strcpy(args[i].in_path, start_dir);
                            strcpy(args[i].in_required, required);
                            strcpy(args[i].out, "NULL");
                        }
                        strcat(args[streams].in_path, file_name);
                        strcat(args[streams].in_path, "\\");
                        streams++;
                    }
                }
            }    
        }

        for(i = 0; i < streams; i++)
        {
            status = pthread_create(&threads[i], NULL, looking_for_file, (void*) &args[i]);
            if (status != 0) 
            {
                printf("\nmain error: can't create thread, status = %d\n", status);
                exit(1);
            }
        }
        for (i = 0; i < streams; i++) 
        {
            status = pthread_join(threads[i], (void**)&status_addr);
            if (status != 0) 
            {
                printf("main error: can't join thread, status = %d\n", status);
                exit(1);
            }
            if(strcmp(args[i].out, "NULL") != 0)
            {
                strcpy(start_args.out, args[i].out);
            }
            printf("joined with address %d\n", status_addr);
        }
        closedir(dir);
        printf("\nFile %s in %s\n", required, start_args.out);
        return 0;
    }
    return 0;
}