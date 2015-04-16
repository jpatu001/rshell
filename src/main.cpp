#include<iostream>
#include<string>
#include<unistd.h> //fork(), execvp(), etc
#include<sys/wait.h> //wait calls
#include<stdio.h> //perror
#include<errno.h> //
#include<dirent.h> //Directory Syscalls
#include<sys/stat.h> //
#include<sys/types.h> //
#include<signal.h> //signal(),kill(), etc
#include<pwd.h> //pwd()
#include<sys/ioctl.h> //ioctl()
#include<stdlib.h>
#include<cstring>
#include<string.h>
#include<vector>
#include<queue>
using namespace std;


//Universal Variables

void parse(char *cmd)
{
	char **arg = (char**) malloc(1000);
	int iter = 0;
	arg[iter] = strtok(cmd," ");	
	iter++;	
	cout << "Gets here\n";
	//if(arg[0]=="exit")	return;
	while((arg[iter]=strtok(NULL," "))!=NULL)
	{
		iter++;
	}
	cout << "Puts NULL\n";
	arg[iter] = NULL;

	int pid = fork();
	if(pid==-1) perror("fork");
	else if(pid==0)
	{
		if(execvp(arg[0],arg)==-1) perror("execvp");
	}
	else{
		int parent = 0;
		if(wait(&parent)==-1) perror("wait");
	}

			
	free(arg);
}
void findOPS(string cmd);

int main()
{

	queue<string> ops;
 	string name;
	//Gets user login name
	name = getlogin();
	if(name=="") perror("getlogin()");

	//Retrieves hostname
	char host[64];
	gethostname(host, 64);	
	if (gethostname(host,sizeof(host)-1)==-1) perror("gethostname()");

	//Removes cs.ucr.edu
	int i = 0;
	bool isFixed = false;
	while(!isFixed)
	{
		if(host[i]=='.'){
			host[i] = '\0';
			isFixed = true;
		}
		++i;
	}
	name = name + '@' + host;
	
	string userIN;
	while(1)
	{
		cout << name << "$ ";
        getline(cin, userIN);
		if(userIN.find('#')!=string::npos) userIN = userIN.substr(0, userIN.find('#'));
		if(userIN == "exit") return 0;
		char* command = (char*)userIN.c_str();
		parse(command);

	}




	return 0;
}


void findOPS(string cmd)
{



}
