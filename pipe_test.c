//pipe_test.c
//Trevor Gudmundsen
//Robert Gonzales
//Natalia 

//program which pipes a given user input through three
//processes, returns to the first one and kills all of them

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define MAX_READ

//function to reverse a string
//returns string in the variable "reversed"
void reverse(char * str, char * reversed) {
    int len = strlen(str) - 1;
    int i = len;
    while (i >= 0) {
        reversed[len - i] = str[i];
        i--;
    }
    reversed[len + 1] = 0;
}

//function which converts str to uppercase
void uppercase(char * str) {
    for (int i = 0; i < strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
}

int main() {
    //pids for process 1, 2, and 3
    pid_t pid1, pid2, pid3;
    pid1 = getpid();
    
    //recieve input string from user
    char input[64];
    printf("process 1 pid: %d \ninput string: ", getpid());
    scanf("%s", &input);
    
    //set up the pipes and perform error checking on bad pipes
    int fd[2];
	int fd2[2];
    int fd3[2];
    if(pipe(&fd[0]) == -1 || pipe(&fd2[0]) == -1 || pipe(&fd3[0]) == -1) {
        perror("error creating pipes");
        exit(1);
    }
    
    //forking for process 2
    pid2 = fork();
    if (pid2 > 0) {
    	//forking for process 3
    	pid3 = fork();
    	//process 1
    	if (pid3 > 0) {
    		//write to child (process 2)
	        close(fd[0]);
	        write(fd[1], input, 64);
			char final[64];
	        //block and wait for process 3 to write to pipe
			read(fd3[0], final, 64);
			printf("\nprocess 1 pid: %d\nrecieved: %s\n", getpid(), final);
			//kill all 3 processes
			printf("\nkilling process 3, pid: %d\n", pid3);
			kill(pid3, SIGQUIT);
			printf("killing process 2, pid: %d\n", pid2);
			kill(pid2, SIGQUIT);
			printf("killing process 1, pid: %d\n", pid1);
			kill(pid1, SIGQUIT);
	    }
	    //process 3
	    else if (pid3 == 0) {
	    	char readstr2[64];
            close(fd2[1]);
            close(fd3[0]);
            //read from parent (process 2)
			read(fd2[0], readstr2, 64);
			printf("\nprocess 3 pid: %d\nrecieved: %s\n", getpid(), readstr2);
            //convert to uppercase
			uppercase(readstr2);
			printf("uppercase: %s\n", readstr2);
            //write back to process 1
			write(fd3[1], readstr2, 64);
	    }
	    else {
	    	perror("error forking");
	    	exit(1);
	    }
	    
    }
    //process 2
    else if (pid2 == 0) {
    	char readstr[64];
        close(fd[1]);
        //read from parent (process 1)
		read(fd[0], readstr, 64);
		printf("\nprocess 2 pid: %d\nrecieved: %s\n", getpid(), readstr);
		char revstr[64];
        //reverse the string
		reverse(readstr, revstr);
		printf("reversed: %s\n", revstr);
        close(fd2[0]);
        //write to child (process 3)
		write(fd2[1], revstr, 64);
    }
    else {
    	perror("error forking");
    	exit(1);
    }
    
    getchar();
    exit(0);
}
