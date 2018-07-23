#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#define BUFFER_SIZE 50
char buffer[BUFFER_SIZE];
int count = 0;
int commandLength[10] = {0};

#define MAX_LINE 80
char *commend_history[10][10];

void handle_SIGINT() {
	write(STDIN_FILENO, buffer, strlen(buffer));
	printf("\nThe history command: \n");
	int i = count;
	for(int count1 = 10; count1 > 0; count1--) {
		for(int j = 0; j < commandLength[i]; j++) {
			printf("%s ", commend_history[i][j]);
		}
		printf("\n");
		i = (i+1)%10;
	}
	printf("\ncommand->");
	fflush(stdout);
}

void setup(char inputBuffer[], char *args[], int *background) {
	int length, //number of char
		i,
		start,//the start char
		ct;//index of comend

	ct = 0;
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

	start = -1;
	if(length == 0) {
		exit(0);
	}
	if(length < 0) {
		printf("read commend error!\n");
		exit(-1);
	}

	for(i = 0; i < length; i++) {
		switch(inputBuffer[i]) {
			case ' ':
			case '\t':
				if(start != -1) {
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				start = -1;
				break;
			case '\n':
				if(start != -1) {
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				args[ct] = NULL;
				break;
			default:
				if(start == -1) {
					start = i;
				} 
				if(inputBuffer[i] == '&') {
					*background = 1;
					inputBuffer[i] = '\0';
				}
		}
	}
	args[ct] = NULL;
}

int main() {
	//malloc the memory to storge the history
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 5; j++) {
			commend_history[i][j] = (char*)malloc(80*sizeof(char));
		}
	}

	//signal to catch
	strcpy(buffer, "\nControl C\n");
	signal(SIGINT, handle_SIGINT);

	char inputBuffer[MAX_LINE];
	int background;
	char *args[MAX_LINE/2 + 1];

	while(1) {
		background = 0;
		printf("COMMAND->");
		fflush(stdout);
		setup(inputBuffer, args, &background);

		//storge command in loop
		if(args[0] != "\n"&&args[0] != NULL) {
			int i;
			for(i = 0; args[i] != NULL; i++) {
				strcpy(commend_history[count][i] , args[i]);
			}
			commandLength[count] = i;
			count = (count+1)%10;
		}

		
		int pid = fork();
		if(pid == 0) {
			execvp(args[0], args);
		} else {
			if(background == 0) {
				wait(NULL);
			} else {
				setup(inputBuffer, args, &background);
			}
		}
	}
	return 0;
}