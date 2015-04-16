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
#include<boost/tokenizer.hpp>
using namespace std;
using namespace boost;

//Universal Variables
//PARSE With Strings and Tokenizer

void exitShell();

void execute(char arg[], char** argv)
{
	if((execvp(arg, argv))==-1){
		perror("execvp");
		_exit(1);
	}	
	/*
	int pid = fork();
	if(pid==-1) perror("fork");
	else if(pid==0){
		if(execvp(arg, argv)==-1) perror("execvp");
	}
	else{
		int parent = 0;
		if(wait(&parent)==-1) perror("wait");
	}
	*/
}

void parse(const string&  cmd)
{
	typedef tokenizer< char_separator<char> > tokenizer;
	char_separator<char> sep(" ");	//Sets char as space
	tokenizer tokens(cmd, sep);		//Sets separator as space " "
	char **arg=(char**)malloc(10000);
	
	//Exit if 'exit' was made"
	tokenizer::iterator iter = tokens.begin(); 
	if((*iter)=="exit"){
		free(arg);
		exit(EXIT_SUCCESS);
		exitShell();
	}

	int i = 0;
	for(iter = tokens.begin(); iter!=tokens.end(); iter++, i++)
	{
		//cout << "TOKEN: " << *iter << endl;
		//strcpy(arg[i], (*iter).c_str()+'\0');
		//arg[i] =(char*)  (*iter).c_str();	
		arg[i] =(char*) strdup((*iter).c_str());
		//cout << "ARG" << i << ": " << arg[i] << endl;
	}
	arg[i] = NULL;
	//cout << arg[0] << " " << arg[1] << endl;
	//execute(arg[0], arg);
	
	int pid = fork();
	if(pid==-1){
		perror("fork");
		exitShell();
	//	if(execvp(arg[0], arg)==-1) perror("execvp");
	}
	else if(pid==0)	//Child Process
	{
		execute(arg[0], arg);
	}
	else{
		int parent = 0;
		if(wait(&parent)==-1){
			 perror("wait");
			 exitShell();
		}
	}

	free(arg);
}

	
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
		//cout << userIN << endl;
		if(userIN.find('#')!=string::npos)
		{
			userIN = userIN.substr(0,userIN.find('#'));

		}
		//cout << userIN << endl;
		
		//Do nothing if empty command
		if(userIN.size()==0){}
		else parse(userIN);

	}




	return 0;
}

void exitShell()
{
	exit(0);
}


/*
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
*/

