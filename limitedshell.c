/*********************************************************************
 * INCLUDE FILES
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <limits.h>
#include <grp.h>


/*********************************************************************
 * DATA STRUCTURES
 *********************************************************************/
#define TRUE 1
/* standard file descriptors */

#define STDIN 1
#define BUFFER_MAX_LENGTH 50                                                    // max buffer [50 chars]
static char* currentDirectory;                                                 // working directory buffer
static char userInput = '\0';
static char buffer[BUFFER_MAX_LENGTH];                                        // user input buffer
static int bufferChars = 0;

static char *commandArgv[5];                                                 // array of strings for given command
static int cmdArgCount = 0;                                                 // command word count



/* shell pid, pgid, default terminal, terminal modes */
static pid_t L_SH_PID;
static pid_t L_SH_PGID;
static int L_SH_TERMINAL, L_SH_IS_INTERACTIVE;

/*********************************************************************
 * USER INPUT FUNCTIONS
 ********************************************************************/
/* gets the next line from the user */
void getTextLine();
/* populates the getline into commandArgv */
void populateCommand();
/* clears the buffer and commandArgv*/
void destroyCommand();

/*********************************************************************
 * PROMPT AND ERROR FUNCTIONS
 *********************************************************************/
/* Screen that will be presented to user*/
void HomeScreen();
/* shell prompt display if an error occurs*/
void L_SHPrompt();

/*********************************************************************
 * THE SHELL FUNCTIONS
 *********************************************************************/
/* Checks if command is built in or not */
void handleUserCommand();
/* checks if command is builtin */
int checkBuiltInCommands();
/* executes the command */
void executeCommand(char *command[], char *file, int newDescriptor);
/* child process created */
void launchJob(char *command[], char *file, int newDescriptor);
/* changes the directory */
void changeDirectory();
/* gets date - internal command */
void internalDate();
/*  used to get user details - internal command */
void userDetails();
/* pwd - external commmand */
void pwd();
/* ifconfig - external command*/
void ifconfig();
/* help - internal command */
void help();
/* initializes the shell */
void init();
/* ls command */
void ls();


/*********************************************************************
 * USER INPUT FUNCTIONS
 *********************************************************************/
/**
 *	gets a line of text from user
 */
void getTextLine(){
        destroyCommand();                                                                       // clean buffer and command line
        while ((userInput != '\n') && (bufferChars < BUFFER_MAX_LENGTH)) {
                buffer[bufferChars++] = userInput;
                userInput = getchar();
        }
        buffer[bufferChars] = 0x00;                                                            // null terminate buffer
        populateCommand();
}

/**
 * space is utilised as a seperator and it populates the array commandArgv
 */
void populateCommand(){
        char* bufferPointer;                                                                  // a pointer to the buffer
        bufferPointer = strtok(buffer, " ");
        while (bufferPointer != NULL) { 
                commandArgv[cmdArgCount] = bufferPointer;                                    // populate commandArgv array
                bufferPointer = strtok(NULL, " ");
                cmdArgCount++;
        }
}

/**
 *	destroys the arrays of chars, letting the user to input the next command line
 */
void destroyCommand(){
        while (cmdArgCount != 0) {
                commandArgv[cmdArgCount] = NULL;
                cmdArgCount--;                                                                       // decrement the number of words in the command line
        }
        bufferChars = 0;                                                                          // erase the number of chars in the buffer
}


/*********************************************************************
 * PROMPT AND ERROR FUNCTIONS
 *********************************************************************/
/**
 *	displays a welcome screen
 */
void HomeScreen(){
		
		printf("\n");
        printf("\tWelcome to Your limieed Shell,  %s\n", getenv("LOGNAME"));
        printf("\tThis liminted shell has a process id of |  %d\n", (int) L_SH_PID);
	printf("\t Please Enter a Command followed by enter.");
        printf("\n\n");
}

/**
 *	displays a nice prompt for the shell
 */
void L_SHPrompt(){
        printf("LSG_%s | ", getenv("LOGNAME"));
}

/**
 * tries to execute builtin and then everything else
 */
void handleUserCommand(){
        if ((checkBuiltInCommands(commandArgv[0])) == 0) {
                printf("This command is not available in your shell please try again\n");
		printf("Having trouble ?\n");
		printf("Enter Help or h \n");
        }
}

/**
 * built-in commands: exit, cd, dt, ud, pw, ifc, help, h
 * returns 	1 if a built-in command is executed,
 * 			0 otherwise
 */
int checkBuiltInCommands(){
        if (strcmp("exit", commandArgv[0]) == 0) {
                exit(EXIT_SUCCESS);
        }
        if (strcmp("cd", commandArgv[0]) == 0) {
                changeDirectory();
                return 1;
        }
		if (strcmp("dt", commandArgv[0]) == 0) {
		         internalDate();
		         return 1;
		}
		if (strcmp("ud", commandArgv[0]) == 0) {
				  userDetails();
				  return 1;
		}
		if (strcmp("pw", commandArgv[0]) == 0) {
					pwd();
					return 1;
		}
		if (strcmp("ifc", commandArgv[0]) == 0) {
					ifconfig();
					return 1;
		}
		if (strcmp("help", commandArgv[0]) == 0) {
					help();
					return 1;
		}
		if (strcmp("h", commandArgv[0]) == 0) {
					help();
					return 1;
		}
		if (strcmp("ls", commandArgv[0]) == 0) {
					ls();
					return 1;
		}
        return 0;
}


/*
 * changes current directory
 */
void changeDirectory(){
        if (commandArgv[1] == NULL) {
	                chdir(getenv("HOME"));
        } else {
                if (chdir(commandArgv[1]) == -1) {
                        printf(" %s: no such directory\n", commandArgv[1]);
                }
        }
}

/**
 * prints out the internal command date
 */
void internalDate() {
		printf("\n");

		time_t t;
		struct tm *tmp;
		char buf[64];
		time(&t);
		tmp = localtime(&t);
		strftime(buf, 64, "The Time and Date is :  %Y%m%d%H%M%S", tmp);
		printf("%s\n", buf);

		printf("\n");
}

/**
 * prints out the user details
 */
void userDetails(){
		printf("\n");

		struct group *gr;
		int ngroups, i;
		gid_t groups[NGROUPS_MAX];

		ngroups = NGROUPS_MAX;
		if ( getgrouplist( getlogin(), getegid(), groups, &ngroups) == -1) {
			printf ("Groups are empty sorry : %d\n", ngroups);
		}

		printf("%d, %s," , geteuid(), getenv("LOGNAME"));
		for (i=0; i < ngroups; i++) {
			 gr = getgrgid(groups[i]);
			printf(" %s," ,  gr->gr_name);
		}
		printf(" %s", getcwd(getenv("HOME"), 1024));
		printf("\n");
		printf("\n");
}

/**
 * pwd
 */
void pwd() {
		printf("\n");
		system("pwd");
		printf("\n");
}

void ls() {
		printf("\n");
		system("ls -a");
		printf("\n");
}

/**
 * help
 */
void help() {

		if (commandArgv[1] != NULL ) {
			if(strcmp(commandArgv[1], "ifc") == 0){
				printf("\n");
				printf("ifc: Displays the ifconfig of the eth0 .\n");
				printf("\n");
			}
			else if (strcmp(commandArgv[1], "pw" ) == 0){
				printf("\n");
				printf("pw | Displays the current working directory.\n");
				printf("\n");
			}
			else if (strcmp(commandArgv[1], "dt" ) == 0){
				printf("\n");
				printf("dt | Displays the date with following format i.e. 20130926215222.\n");
				printf("\n");
			}
			else if (strcmp(commandArgv[1], "ud" ) == 0){
				printf("\n");
				printf("ud | Displays the user details in the format | userID, groupID, username, groupname, iNode of users home directory.\n");
				printf("\n");
			}
			else if (strcmp(commandArgv[1], "ls" ) ==0) {
				printf("\n");
				printf("ls | Displays the files in current directory.\n");
				printf("\n");
			}
			else if (strcmp(commandArgv[1], "exit" ) == 0){
				printf("\n");
				printf("exit | exit L_SH.\n");
				printf("\n");
			}
			else {
				printf("\n");
				printf("%s | has no help implemented.\n", commandArgv[1]);
				printf("\n");

			}
		}
		else {
				if (commandArgv[1] == NULL) {
					printf("\n");
					printf("ud | Displays the userid, groupid, username and groupname.\n");
					printf("dt | Displays the current date i.e. 20130926215222.\n");
					printf("pw | Displays the current working directory.\n");
					printf("ifc | Displays the ifconfig of the eth0 by default, you can specify your own interface.\n");
					printf("ls | Displays the current files in the directory.\n");
					printf("exit | Will exit limitedshell /  L_SH.\n");
					printf("\n");
				}
			}
}

/**
 * ifconfig
 */
void ifconfig() {
	char command[100];
	if (commandArgv[1] == NULL) {
		printf("\n");
		system("ifconfig eth0");
		printf("\n");
	} else {
	    if (commandArgv[1] != NULL) {                                  // go to the new directory
	       	printf("\n");
			sprintf(command, "ifconfig %s", commandArgv[1]);
			system(command);
			printf("\n");
	    }
	}
}



/**
 * initializes variables and enables job control
 */
void init(){
        L_SH_PID = getpid();                                    // retrieve the pid of the shell
        L_SH_TERMINAL = STDIN_FILENO;                           // terminal = STDIN
        L_SH_IS_INTERACTIVE = isatty(L_SH_TERMINAL);            // the shell is interactive if STDIN is the terminal

        if (L_SH_IS_INTERACTIVE) {                                                 // is the shell interactive?
                while (tcgetpgrp(L_SH_TERMINAL) != (L_SH_PGID = getpgrp()))
                        kill(L_SH_PID, SIGTTIN);                                                    // make sure we are in the foreground

                /**
                 * ignore all the job control stop signals and install custom signal handlers
                 */
                signal(SIGQUIT, SIG_IGN);
                signal(SIGTTOU, SIG_IGN);
                signal(SIGTTIN, SIG_IGN);
                signal(SIGTSTP, SIG_IGN);
                signal(SIGINT, SIG_IGN);

                setpgid(L_SH_PID, L_SH_PID);                                         // we make the shell process as new process group leader
                L_SH_PGID = getpgrp();
                if (L_SH_PID != L_SH_PGID) {
                        printf("Error");
                        exit(EXIT_FAILURE);
                }
               currentDirectory = (char*) calloc(1024, sizeof(char));
        } else {
                printf("Exiting..\n");
                exit(EXIT_FAILURE);
        }
}

int main(int argc, char **argv, char **envp){
        init();
        HomeScreen();											//display homescreen
        L_SHPrompt();
        while (TRUE) {
                userInput = getchar();
                switch (userInput) {
                case '\n':                                                                               // if the user hits the enter key
                        L_SHPrompt();                                                                     // nothing happens
                        break;										//Breaks out from loop
                default:
                        getTextLine();                                                                  // store user input in buffer
                        handleUserCommand();                                                           // execute user command
                        L_SHPrompt();                                                                  // print the prompt
                        break;										// Breaks out from  loop
                }
        }
        printf("\n");
        return 0;
}
