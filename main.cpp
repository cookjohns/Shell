//  main.cpp
//  Created by John Cook on 2/7/16.

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

using namespace std;

/* Get inputs and puts it into argv */
void getInput(char* input, char* &programToRun, char** argv, char* &redirectFileName, int &mode) {
    cout << "$ ";
    cin.getline(input, 255);
    programToRun = strtok(input, " ");
    char* temp   = programToRun;
    int argCount = 0;
    int i        = 0;
    while (temp != NULL) { // fill argv from (tokenized) input
        argv[i++] = temp;
        temp = strtok(NULL, " ");
        if (temp != NULL && mode == 2) redirectFileName = temp;
        if (temp != NULL && strcmp(temp, ">") == 0) mode = 2;
        argCount++;
    }
    mode == 1 ? argv[argCount] = NULL : argv[argCount-2] = NULL; // null terminate argv
}

/* Calls execvp to execute param programToRun with arguments in param argv */
void execute(char* programToRun, char** argv, char* redirectFileName, int mode) {
    pid_t childPid;
    int status;
    /* fork with error check */
    if ((childPid = fork()) < 0) {
        printf("Fork error\n");
    }
    /* execvp with error check */
    else if (childPid == 0) {
        if (mode == 2) {
            cout << redirectFileName;
            close(STDOUT_FILENO);
            open (redirectFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        }
        if (execvp(programToRun, argv) < 0) {
            cout << programToRun;
            printf(": No such file or directory.\n");
        }
        exit(1);
    }
    /* wait for child */
    else {
        while (wait(&status) != childPid) {}
    }
}

int main() {
    char* programToRun;
    char* redirectFileName = NULL;
    int   mode             = 1;
    
    while (1) { // infinite loop
        char** argv = new char*[255];
        mode        = 1;
        char input[255];
        memset(input, 0, sizeof(input)); // clear input
        
        getInput(input, programToRun, argv, redirectFileName, mode);
        execute(programToRun, argv, redirectFileName, mode);
    }
}
