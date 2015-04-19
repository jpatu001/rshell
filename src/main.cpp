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

void execute(const string& cmd);
void exitShell();
void parse(const string&  cmd, queue<string>& ops);
void findOPS(queue<string>& ops, string& cmd);
bool validIN(string& in);
	
bool cmdWorked = false; //Global variable to track if command succeeded
bool failed = false;
string prevConn = "";

int main()
{

 	string name;
	//Gets user login name
	name = getlogin();
	if(name=="\0") perror("getlogin()");

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
		//Do nothing if empty command
		if(!validIN(userIN)){}
		if(!validIN(userIN)){} //Gets rid of all whitespaces entry 
		else{
			queue<string> ops;
			findOPS(ops,userIN);
			parse(userIN, ops);
		}	

	}




	return 0;
}

void parse(const string&  cmd,queue<string>& ops)
{
	char* command =(char*)  cmd.c_str();
	char* tok;
	string prev;
	if(ops.size()!=0){
		bool firstRound = true;
		do{
				//First command
				if(firstRound){
				tok = strtok(command, (ops.front()).c_str());
				execute(tok);
				if(ops.front()=="&&" && cmdWorked){
					firstRound=false;
					continue;
				}	
				else if(ops.front()==";"){
					firstRound = false;
					continue;
				}
				else if(ops.front()=="||" && !cmdWorked){
					firstRound = false;
					continue;
				}	
				
				else if(ops.front()=="||" && cmdWorked){
					firstRound = false;
					continue;
				}
				else return;
			}
			//Following commands
			else if (ops.size()>1 && !firstRound){
				if(ops.front()=="&&" && cmdWorked){
					ops.pop();	
					string tmp = ops.front();
					tmp = tmp + "&";	//Adds prev to delimiters
					tok = strtok(NULL, tmp.c_str());
					execute(tok);
					continue;
				}	
				else if(ops.front()==";"){ // && (cmdWorked||!cmdWorked)
					ops.pop();
					tok = strtok(NULL, (ops.front()).c_str());
					execute(tok);
					continue;
				}
				else if(ops.front()=="||" && !cmdWorked){
					ops.pop();
					string tmp = ops.front();
					tmp = tmp + "|";	//Adds prev to delimters
					tok = strtok(NULL, (ops.front()).c_str());
					execute(tok);
					continue;
				}	
				else if(ops.front()=="||" && cmdWorked)
				{
					ops.pop();
					strtok(NULL, (ops.front()).c_str());
					continue;
				}	
				
				else continue;
			}
			else
			{
				if(ops.front()=="&&" && !cmdWorked) return; //If ops is && but previous failed
				if(ops.front()=="||" && cmdWorked)	return; 
				
				ops.pop();
				char** tmp = (char**) malloc(10000);
				int j = 0;
				//TOKENIZE
				while(tok!=NULL)	
				{
					tok = strtok(NULL, " |&");
					tmp[j] = tok;
					j++;
				}
				tmp[j] = NULL;

				string cmnd;
				for(int i = 0; tmp[i]!=NULL; i++)
				{
					cmnd+=tmp[i];
					cmnd+=" ";
				}
				free(tmp);
				if(validIN(cmnd)) execute(cmnd); //Checks if it is valid
				continue;
			}
		}while(tok!=NULL && ops.size()!=0);
	}

	else{//SINGLE COMMANDS WITHOUT CONNECTORS
		execute(cmd);
	}
}

void execute(const string& cmd)
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

	//Parses the command with spaces
	int i = 0;
	for(iter = tokens.begin(); iter!=tokens.end(); iter++, i++)
	{
		arg[i] =(char*) strdup((*iter).c_str());
	}
	arg[i] = NULL;
	int pid = fork();
	if(pid==-1){//Fork Error
		perror("fork");
		cmdWorked = false;
		free(arg);
		exitShell();
	}
	else if(pid==0)//Child Process
	{
		if(execvp(arg[0], arg)==-1){ //Execvp Failed
		perror("execvp");
		free(arg);
		exit(3); //Sets exit status to 3
		//_exit(2);
		}	

	}
	else{//Parent
		int parent = 0;
		if(wait(&parent)==-1){//Wait Error
			perror("wait");
			cmdWorked = false;
			free(arg); //Free arg before exitting
			exitShell();
		}
		
	
		if(WIFEXITED(parent) && WEXITSTATUS(parent)==3) { //Sets bool to false if the command didnt execute
			cmdWorked = false;
		}
		else cmdWorked = true;	//Else it executed correctly
	}

	free(arg);


}

void findOPS(queue<string>& ops, string& cmd){
	//Gathers all operations and push to queue in order
	for(unsigned int i = 0; i < cmd.size()-1; i++)
	{
		if(cmd.at(i)=='&' && cmd.at(i+1)=='&')
		{
			ops.push("&&");
			i++;
		}
		else if(cmd.at(i)=='|' && cmd.at(i+1)=='|')
		{
			ops.push("||");
			i++;
		}
		else if(cmd.at(i)==';')
		{
			ops.push(";");
		}

	}
}

bool validIN(string& in)
{
	if(in.size()==0) return false;
	if(in.find_first_not_of(' ') == string::npos) return false;
	else return true;
}

void exitShell()
{
	exit(0);
}
