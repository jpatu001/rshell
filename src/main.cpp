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

void execute(char arg[], char** argv);
void exitShell();
void parse(const string&  cmd);
void findOPS(queue<string>& ops, string& cmd);
	
int main()
{

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
		//Removes everything after '#' (comment)
		if(userIN.find('#')!=string::npos)
		{
			userIN = userIN.substr(0,userIN.find('#'));

		}
		/*
		//REMOVE AFTER TESTING==============
		if(userIN=="exit") exitShell();
		queue<string> ops;
		findOPS(ops,userIN);
		cout << "Connectors: "; 
		while(!ops.empty()){
			cout << ops.front() << " ";
			ops.pop();
		}
		cout << endl;
		//REMOVE WHEN DONE^^^^^^^^^^^^^^^^^^^
		*/
		//Do nothing if empty command
		if(userIN.size()==0){}
		else{
			queue<string> ops;
			findOPS(ops,userIN);
			parse(userIN);
		}	

	}




	return 0;
}

void parse(const string&  cmd)
{
	typedef tokenizer< char_separator<char> > tokenizer;
	char_separator<char> sep(" ");	//Sets char as space
	tokenizer tokens(cmd, sep);		//Sets separator as space " "
	char **arg=(char**)malloc(100000000); //Allocate space for 100m Command Line
	
	//Exit if 'exit' was entered"
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
		arg[i] =(char*) strdup((*iter).c_str());
		//cout << "ARG" << i << ": " << arg[i] << endl;
	}
	arg[i] = NULL;
	//cout << arg[0] << " " << arg[1] << endl;
	//execute(arg[0], arg);
	
	int pid = fork();
	if(pid==-1){//Fork Error
		perror("fork");
		free(arg);
		exitShell();
	}
	else if(pid==0)//Child Process
	{
		execute(arg[0], arg);
	}
	else{//Parent
		int parent = 0;
		if(wait(&parent)==-1){//Wait Error
			perror("wait");
			free(arg); //Free arg before exitting
			exitShell();
		}
	}

	free(arg);
}

void execute(char arg[], char** argv)
{
	if((execvp(arg, argv))==-1){
		perror("execvp");
		_exit(1);
	}	
}

void findOPS(queue<string>& ops, string& cmd){
	for(unsigned int i = 0; i < cmd.size()-1; i++)
	{
		if(cmd.at(i)=='&' && cmd.at(i+1)=='&')
		{
			ops.push("&&");
		}
		else if(cmd.at(i)=='|' && cmd.at(i+1)=='|')
		{
			ops.push("||");
		}

	}
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

