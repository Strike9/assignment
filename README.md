# assignment

#Systems Integration
##Assignment 1 - Custom Shell
###Author: Robert Murphy - C13741321

##Introduction
Custom limited shell is developed for the purpose of having a custom shell
for particular users of a the system. This shell has limited functionalities 
and options which restricts the users functionality,
Meaning the user are not able to use commands that are not in this custom shell.

This shell is developed using C programming language. The program provides limited 
commands for the users. For example, 
when the user entered the custom shell command "pw", the default shell
command "pwd" will be ran and the data will be display on the terminal.

##Installation
Installing this shell for a user is quite simple and can be done by following
the steps bellow:

**1 - Clone Repository**
- This is the most recommended way to install this custom shell and is done following the steps bellow:
	- Open terminal and enter command: sudo su
	- After entering your password, enter command: cd /
		- This will bring you to your main directory
	- Then enter: git clone "https://github.com/Strike9/assignment.git"
	- This repository will be cloned into the directory you are in.
	- This repository should contain a file named limitedShell.c which is thefile for the custom shell

**2.1 - Creating Users**
- *If there are users avaible to set-up the limited shell, you can skip this step but 
look at the following step (2.2) to make sure you have the right configuration.	
- To create a new user, enter the following commands:
	- useradd -m username
		- e.g. useradd -m Lecturer 
		- Flag -m will create and set a home directory for the use and named as the user.
	- passwd username
		- e.g. passwd CA1
		- This will create a password for that user.
- To switch user, enter the command: su username
	- e.g. su Lecturer

**2.2 - User Shell Configuration**
- This step will change the default shell to custom shell of a user.
	- Enter command: sudo chsh -s /DirectoryOfClone/assignment/limitedshell username	
		- This will configure your shell.
	
##How It Works
- The shell is ran when the particular user logs in.

- It is developed that when the user logs in, the current directory will automatically switch to that  
directory in which they cloned into. This way, that user will have no access to other user accounts that can be found in /home/ directory.

- When the user entered a command, the shell checks whether that command does or doesnt have a tail. If no tail 
is provided, it will go through the case statement to run that command or call a function to run that command. 
If the command contains tail(s), it will also go throught the case statement then call the fucntion which runs 
that command after checking for any errors.

- Users can log out of their system or the custom shell by entering the command: **exit**. Then the user will
see a logging out message and what that user have done or accesed within its account will not be seen by other users.

