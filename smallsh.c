/*
Andres Dominguez
CS - 344 Operating Systems
Program 3
Due Date: 2-23-2015
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#define SIZE 512 // size and length decleration for arguemnts

char input[SIZE];
char delim[30] = " \t ( ) | ; \n    ";

int main()
{
    char *args[SIZE];
    int i;
    int status = 0;
    pid_t f;
    int f_list[1000];
    int f_counter = 0;
    int background;
    int counter;
    
    // while loop that will continue to run until the shell is exited
    while (1){
          
        counter = 0;  
        background = 0;
        printf(": ");
        
        // check to make sure that the input is not null
        if(fgets(input, sizeof(input), stdin) == NULL){
            break;
        }

        // put the first argument entered into the array
        args[0] = strtok(input, delim); 

        // check to see if that argument is null and if so continue
        // and display the command prompt again
        if (args[0] == NULL){
                    
            continue; 
        }

        // loop to tokenize the arguemnts entered
        for (i = 1; i < SIZE; i++){ 
                  
            args[i] = strtok(NULL, delim);
            counter++;
            
            if (args[i] == NULL){
                break;
            }
        }

        // built in command for changing directory
        if (strcmp(args[0], "cd") == 0){

            // if nothing is entered after "cd" then go to the home directory
            if (args[1] == NULL){
                        
                chdir(getenv("HOME"));
            // if the directory does not exist display an error
            }else if(chdir(args[1]) == -1){
                  
                  printf("%s\n", "No such file or directory.");
            }
        }
        // built in command to exit the shell
        else if (strcmp(args[0], "exit") == 0){
             
            printf("%s\n", "Now Exiting.");
            exit(0);
        }
        // built in command to recognize lines starting with an "#" as comments
        else if(strcmp(args[0], "#") == 0){
             
             continue;
        }
        // built in command for status. Return the last processes exit status
        else if(strcmp(args[0], "status") == 0){
             
             waitpid(f, &status, 0);
             printf("Exit status is %d\n", WEXITSTATUS(status));
        }
        // if a built in command was not entered..
        else{
             // check to see if the "&" symbol was used to indicate a background
             // process
             if(strcmp(args[counter-1], "&") == 0){
                 
                 // set background signal                        
                 background = 1;
                 args[counter-1] = NULL;                 
             }
        
             f = fork();
             
             // parent process and we are in the background
             if(f != 0 && background == 1){
                          
                 f_list[f_counter] = f;
                 f_counter++;               
                 printf("We are in the background!\n");
                 printf("Process ID: %d\n", getpid());
             }else if(f == 0 && background == 1){ // if were a child in the background
                  
                  // close the stdin for the background process
                  fclose(stdin);
                  fopen("/dev/null", "r");
                  
                  if(execvp(args[0], args)){
                       
                       printf("%s\n", args[0]);
                       puts(strerror(errno));
                       exit(1);
                  }
             }
             // parent process not in the background
             if(f != 0 && background == 0){
                  
                  //waiting for child
                  fflush(stdout);
                  f = waitpid(f, &status, 0);
             }else if (f == 0 && background == 0){// child process not in the background
                
                // loop to check for read and write operations
                for(i = 0; i < SIZE; i++){
                    
                    if(args[i] == NULL){
                            
                        break;
                    }
                    // redirect standard output to a file
                    if(strcmp(args[i], ">") == 0){
                        
                        args[i] = strtok(args[i], ">");
                            
                        if(args[i+1] == NULL){
                                
                            printf("Did not recognize file. Please try again.\n");
                        }
                            
                        freopen(args[i+1], "w+", stdout);
                        
                        if(execvp(args[0], args) < 0){
                            
                            printf("Error. Could not execute that command.\n");
                            exit(1);
                        }
                    // redirect standard input from a file
                    }else if(strcmp(args[i], "<") == 0){
                        
                        args[i] = strtok(args[i], "<");
                            
                        if(args[i+1] == NULL){
                                
                            printf("Did not recognize file. Please try again.\n");
                        }
                            
                        if(freopen(args[i+1], "r", stdin) < 0){
                            printf("Error opening file.\n");
                        }
                        
                        if(execvp(args[0], args) < 0){
                            
                            printf("Error. Could not execute that command.\n");
                            exit(1);
                        }
                    }
                    // append standard output to a file
                    else if (strcmp(args[i], ">>") == 0){
                        
                        args[i] = strtok(args[i], ">>");
                            
                        if(args[i+1] == NULL){
                                
                            printf("Did not recognize file. Please try again.\n");
                        }
                            
                        if(freopen(args[i+1], "a", stdout) < 0){
                                
                            printf("Error.\n");
                            exit(1);
                        }
                            
                        if(execvp(args[0], args) < 0){
                            
                            printf("Error. Could not execute that command.\n");
                            exit(1);
                        }
                    }
                }    
                // check to see if the command could be executed
                if(execvp(args[0], args) < 0){
                                       
                    printf("Error. Could not execute that command.\n");
                    exit(1);
                }
             // check to see if fork failed
             }
             else if(f < 0){
                
                 printf("Error launching process. Please try again later.");
                 exit(1);
             }
             else{
                 // wait call to continue processes if things are running in the background
                 waitpid(f, &status, WNOHANG);
                 
                 if(background == 1){
                      
                     printf("Exit status for background process is %d\n", WEXITSTATUS(status));
                 }
             }
        }
        fflush(stdout);
    }
    return 0;
}
