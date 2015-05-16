#include<iostream>
#include<string>
#include<unistd.h>	//fork(), execvp(), etc
#include<sys/wait.h>	//wait calls
#include<stdio.h>	//perror
#include<errno.h>
#include<dirent.h>	//Directory Syscalls
#include<sys/stat.h> 
#include<sys/types.h>
#include<signal.h>	//signal(),kill(), etc
#include<pwd.h>		//pwd()
#include<sys/ioctl.h>	//ioctl()
#include<stdlib.h>
#include<cstring>
#include<string.h>
#include<vector>
#include<queue>
#include<fcntl.h>
#include<boost/tokenizer.hpp>
using namespace std;
using namespace boost;

void execute(const string& cmd);
void exitShell();
void parse(const string&  cmd, queue<string>& ops);//For regular connectors
void parse2(const string& cmd, queue<string>&ops);//For Piping and IO
void findOPS(queue<string>& ops, const string& cmd);
bool validIN(string& in);	
void removeSpaces(string& str);
void inputRed(const char* in, const char* in1, const char* in2, bool isAppend);
void stringRed(const char* in, const char* in1);
void outputRed(const char* in, const char* in2);
void outputRedAppend(const char* in, const char* in2);
//void piping(const char* in, const char* in1);
void piping(queue<string> cmd);
bool cmdWorked = false;	//Global variable to track if command succeeded
bool isPporRd = false; //Set to true if the connectors are pipes or IO redirection	
bool isErrRed = false; //Set to true if redirection with specific # is done eg.	`command #> file`
int fileDescriptor = 2; //Saves the filedescriptor given by the user, used with isErrRed

int main()
{

 	string name;
	//Gets user login name
	name = getlogin();
	if(name=="\0") perror("getlogin()");
	//Retrieves hostname
	char host[64];
	if ((gethostname(host, 64))==-1) perror("gethostname()");

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
		isPporRd = false;
		isErrRed = false;
		fileDescriptor = 2;
		cout << name << "$ ";
        getline(cin, userIN);
		//Exit right away if exit was typed
		if(userIN=="exit" || userIN=="Exit") exitShell();
		//Removes everything after '#' (comment)
		if(userIN.find('#')!=string::npos) userIN = userIN.substr(0,userIN.find('#'));
		//Do nothing if empty command
		if(!validIN(userIN)){}//Gets rid of all whitespaces entry 
		else{
			queue<string> ops;
			findOPS(ops, userIN);
			if(isPporRd) parse2(userIN, ops);
			else parse(userIN, ops);
		}	
	}
	return 0;
}

void parse2(const string& cmd, queue<string>&ops)
{
	typedef tokenizer< char_separator<char> > tokenizer;
	char_separator<char> sep("<>&|;");//Sets char as space
	tokenizer tokens(cmd, sep);//Sets separator as space " "
	char* tok;
	char* tok2;
	char* tok3;
	for(tokenizer::iterator itr = tokens.begin(); itr != tokens.end(); itr++)
	{
			if(ops.front()=="<"){
				ops.pop();
				tok = (char*) strdup((*itr).c_str());
				tok2 = (char*) strdup((*(++itr)).c_str());
				if(ops.size()>0 && ops.front()==">"){
					tok3 = (char*) strdup((*(++itr)).c_str());
					ops.pop();
					inputRed(tok,tok2,tok3,false);
					continue;
				}
				else if(ops.size()>0 && ops.front()==">>"){
					tok3 = (char*) strdup((*(++itr)).c_str());
					ops.pop();
					inputRed(tok,tok2,tok3,true);
					continue;
				}
				else{
					inputRed(tok, tok2, "nofilegiven",false);
					continue;
				}
			}
			else if(ops.front()=="<<<"){
				ops.pop();
				tok = (char*) strdup((*itr).c_str());
				tok2 = (char*) strdup((*(++itr)).c_str());
				stringRed(tok,tok2);
				continue;
			}
			else if(ops.front()==">"){
				ops.pop();
				tok = (char*) strdup((*itr).c_str());
				tok2 = (char*) strdup((*(++itr)).c_str());
				outputRed(tok,tok2);
				continue;
			}
			else if(ops.front()==">>"){
				ops.pop();
				tok = (char*) strdup((*itr).c_str());
				tok2 = (char*) strdup((*(++itr)).c_str());
				outputRedAppend(tok,tok2);
				continue;
			}
			else if(ops.front()=="|"){
				queue<string> pipethis;
				while((ops.size() > 0) && ops.front()=="|"){
					ops.pop();
					pipethis.push(*itr);
					++itr;
				}
				pipethis.push(*itr);
				piping(pipethis);
				continue;
			}
			else break;
	}
}

void piping(queue<string> cmd)
{
	int oldfdi, oldfdo;
	if(-1==(oldfdi = dup(0))) perror("dup(oldfdout)");
	if(-1==(oldfdo = dup(1))) perror("dup(oldfdin)");
	int fd[2];
	if(-1==(pipe(fd))) perror("pipe()");	

	int fd2[2];
	if(-1==(pipe(fd2))) perror("pipe()");
	//First command, put output in pipe======================================
	string command(cmd.front());
	cmd.pop();
	if(-1==(close(1))) perror("close(1)");
	if(-1==(dup(fd[1]))) perror("dup(fdout)");//STDOUT -> IN-PIPE
	execute(command);
	if(-1==(close(fd[1]))) perror("close(fd[1]]");//CLOSE ONE END OF PIPE
	//=========================================================================	
	bool flag = true;
	while(cmd.size()>1)
	{
		if(!flag) pipe(fd);
		else pipe(fd2);
		if(flag){
			string command2 = cmd.front();
			cmd.pop();
			if(-1==(close(0))) perror("close(0)");
			if(-1==(dup(fd[0]))) perror("dup(fd[0])"); //STDIN -> OUT-PIPE
			if(-1==(close(1))) perror("close(1)");
			if(-1==(dup(fd2[1]))) perror("dup(fd2[1])"); //STDOUT -> OUT-PIPE2
			execute(command2);
			if(-1==(close(fd[0]))) perror("close(fd[0])");
			if(-1==(close(fd2[1]))) perror("close(fd2[1])");
			flag = false;
		}
		else//Read from fd2 write to fd
		{
			string command2 = cmd.front();
			cmd.pop();
			if(-1==(close(0))) perror("close(0)");
			if(-1==(dup(fd2[0]))) perror("dup(fd2[0])"); //STDIN -> OUT-PIPE2
			if(-1==(close(1))) perror("close(1)");
			if(-1==(dup(fd[1]))) perror("dup(fd[1])"); //STDOUT -> OUT-PIPE
			execute(command2);
			if(-1==(close(fd2[0]))) perror("close(fd2[0])");
			if(-1==(close(fd[1]))) perror("close(fd[1])");
			flag = true;
		}
	}
	//Last part, restore all out, take input from pipe, and output to stdout
	command = cmd.front();
	cmd.pop();
	if(-1==(close(0))) perror("close(0)");
	if(!flag){
		if(-1==(dup(fd2[0]))) perror("dup(fd2[0])"); //STDIN -> OUT-PIPE2
	}
	else{
		if(-1==(dup(fd[0]))) perror("dup(fd[0])"); //STDIN -> OUT-PIPE
	}
	if(-1==(dup2(oldfdo,1))) perror("dup2");//Restore stdout
	if(-1==(close(oldfdo))) perror("close(oldfdo)");//Close oldfdo
	execute(command);
	if(-1==(dup2(oldfdi,0))) perror("dup(oldfdi)");//Restore stdin
	if(-1==(close(oldfdi))) perror("close(olfdi)");	
	if(flag){
		if(-1==(close(fd[0]))) perror("close(fd[0]");//CLOSE ONE END OF PIPE
	}
	else{
		if(-1==(close(fd2[0]))) perror("close(fd[0]");//CLOSE ONE END OF PIPE
	}	
}
/*
void piping(const char* in, const char* in1)
{
	string cmd1(in);
	string cmd2(in1);
	int fds[2];
	int oldfdi;
	int oldfdo;
	if(-1==pipe(fds)) perror("pipe()");
	if(-1==(oldfdo=dup(1))) perror("dup");//Backup of OUT
	if(-1==(oldfdi=dup(0))) perror("dup");//Backup of IN
	if(-1==(dup2(fds[1],1))) perror("dup2()");//Out of first command goes to pipe
	execute(cmd1);
	//Restore Stdout
	if(-1==dup2(oldfdo,1)) perror("dup2()");//Restore stdout
	if(-1==close(oldfdo)) perror("close()");
	if(-1==close(fds[1])) perror("close()");	
	if(-1==(dup2(fds[0],0))) perror("dup2()");//In of second command comes from pipe
	execute(cmd2);
	//Restoring stdin
	if(-1==dup2(oldfdi,0)) perror("dup2()");//Restore stdin
	if(-1==close(oldfdi)) perror("close()");
	if(-1==close(fds[0])) perror("close()");	
}
*/
void parse(const string& cmd,queue<string>& ops)
{
	char* command =(char*)  cmd.c_str();
	char* tok;
	bool firstRound = true;
	if(ops.size()!=0){
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
					tmp = tmp + "&";//Adds prev to delimiters
					tok = strtok(NULL, tmp.c_str());
					execute(tok);
					continue;
				}	
				else if(ops.front()==";"){// && (cmdWorked||!cmdWorked)
					ops.pop();
					string tmp = ops.front();
					tmp = tmp + ";";
					tok = strtok(NULL, (ops.front()).c_str());
					execute(tok);
					continue;
				}
				else if(ops.front()=="||" && !cmdWorked){
					ops.pop();
					string tmp = ops.front();
					tmp = tmp + "|";//Adds prev to delimters
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
			else//Last argument
			{
				if(ops.front()=="&&" && !cmdWorked) return;//If ops is && but previous failed
				if(ops.front()=="||" && cmdWorked)	return; 
				
				ops.pop();
				char** tmp = (char**) malloc(10000);
				int j = 0;

				//TOKENIZE
				while(tok!=NULL)	
				{
					tok = strtok(NULL, " ;|&");
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
				if(validIN(cmnd)) execute(cmnd);//Checks if it is valid
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
	char_separator<char> sep(" ;");//Sets char as space
	tokenizer tokens(cmd, sep);//Sets separator as space " "
	char **arg=(char**)malloc(100000000);//Allocate space for 100m Command Line
	//===Exit if 'exit' was entered"===========================================
	tokenizer::iterator iter = tokens.begin(); 
	if((*iter)=="exit"){
		free(arg);
		exit(EXIT_SUCCESS);
		exitShell();
	}
	//===Parses the command with spaces========================================
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
		if(execvp(arg[0], arg)==-1){//Execvp Failed
		perror("execvp");
		free(arg);
		exit(3);//Sets exit status to 3
		}	
	}
	else{//Parent
		int parent = 0;
		if(wait(&parent)==-1){//Wait Error
			perror("wait");
			cmdWorked = false;
			free(arg);//Free arg before exitting
			exitShell();
		}
		if(WIFEXITED(parent) && WEXITSTATUS(parent)==3) {//Sets bool to false if the command didnt execute
			cmdWorked = false;
		}
		else cmdWorked = true;//Else it executed correctly
	}
	free(arg);
}

void findOPS(queue<string>& ops, const string& cmd){
	//Gathers all operations and push to queue in order
	for(unsigned int i = 0; i < cmd.size()-1; i++)
	{
		//AND
		if(cmd.at(i)=='&' && cmd.at(i+1)=='&')
		{
			ops.push("&&");
			i++;
		}
		//OR
		else if(cmd.at(i)=='|' && cmd.at(i+1)=='|')
		{
			ops.push("||");
			i++;
		}
		//SemiColon
		else if(cmd.at(i)==';')
		{
			ops.push(";");
		}
		//PIPE
		else if(cmd.at(i)=='|' && cmd.at(i+1)!='|' && cmd.at(i-1)!='|')
		{
			isPporRd = true;	
			ops.push("|");
		}
		//INPUT REDIRECT
		else if(cmd.at(i)=='<' && cmd.at(i+1)!='<' && cmd.at(i+2)!='<')
		{
			isPporRd = true;	
			ops.push("<");
		}
		else if(cmd.at(i)=='<' && cmd.at(i+1)=='<' && cmd.at(i+2)=='<')
		{
			isPporRd = true;	
			ops.push("<<<");
			i+=2;
		}
		//OUTPUT REDIRECT
		else if(cmd.at(i)=='>' && cmd.at(i+1)=='>')
		{
			isPporRd = true;	
			if(cmd.at(i-1)>='0' && cmd.at(i-1)<='9'){
				isErrRed = true;
				fileDescriptor = cmd.at(i-1) - '0';
			}
			ops.push(">>");
			i++;
		}
		else if(cmd.at(i)=='>' && cmd.at(i+1)!='>' && cmd.at(i-1)!='>')
		{
			isPporRd = true;	
			if(cmd.at(i-1)>='0' && cmd.at(i-1)<='9'){
				isErrRed = true;
				fileDescriptor = cmd.at(i-1) - '0';
			}
			ops.push(">");
		}
	}
}

void removeSpaces(string& str)
{
	for(string::iterator i=str.begin(); i!=str.end(); i++)
	{
		if(*i==' ') {str.erase(i); i--;}
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

void inputRed(const char* in, const char* in1, const char* in2, bool isAppend)
{
	string cmd(in);
	string file(in1);
	string file2(in2);
	if(file.size()==0)
	{
		cerr << "syntax error near unexpected token 'newline'" << endl;
		return;
	}
	if(cmd.size()!=0) removeSpaces(cmd);
	if(file.size()!=0) removeSpaces(file);
	if(file2.size()!=0) removeSpaces(file2);
	//Opens file for input
	int fd;
	if(-1==(fd = open(file.c_str(), O_RDONLY)))
	{
		perror("open()");
		return;	
	}
	int fd2, oldfd2;
	//Changes stdout in file was given
	if(file2!="nofilegiven")
	{
		if(!isAppend){
			if(-1==(fd2 = open(file2.c_str(), O_WRONLY|O_CREAT|O_TRUNC,0666)))
			{
				perror("open()");
				//exit(1);
			}
		}
		else{
			if(-1==(fd2 = open(file2.c_str(), O_WRONLY|O_CREAT|O_APPEND,0666)))
			{
				perror("open()");
				exit(1);
			}
		}
		//Copies old file descriptor
		if(-1==(oldfd2 = dup(1))){
			perror("dup");
			exit(1);
		}
		//Redirect output
		if(-1==dup2(fd2,1)){
			perror("dup2");
			exit(1);
		}
		//Close other fd2
		if(-1==close(fd2)){
			perror("close()");
			exit(1);
		}
	}
	//Copies old file descriptor
	int oldfd;
	if(-1==(oldfd = dup(0))){
		perror("dup");
		exit(1);
	}
	//Redirect input 
	if(-1==dup2(fd,0)){
		perror("dup2");
		exit(1);
	}
	//Close other fd
	if(-1==close(fd)){
		perror("close()");
		exit(1);
	}
	execute(cmd);
	//Closes stdout
	if(file2!="nofilegiven")
	{
		//Restore stdout
		if(-1==dup2(oldfd2,1)){
			perror("dup2()");
			exit(1);
		}
		if(-1==close(oldfd2)){
			perror("close()");
			exit(1);
		}
	}
	//Restore stdin
	if(-1==dup2(oldfd,0)){
		perror("dup2()");
		exit(1);
	}
	if(-1==close(oldfd)){
		perror("close()");
		exit(1);
	}
}
void stringRed(const char* in, const char* in1)
{
	string cmd(in);
	string exec = "echo ";
	string tmp(in1);
	tmp = tmp.substr(tmp.find('"')+1, tmp.size()-1);//Removes first "
	if(tmp.at(tmp.size()-1)=='"') tmp.erase(tmp.size()-1);//Removes last "
	exec = exec + tmp; //Concatonate echo before the string
	
	int fds[2];
	int oldfdi;
	int oldfdo;
	if(-1==pipe(fds)) perror("pipe()");
	//Left Side
	if(-1==(oldfdo=dup(1))) perror("dup");//Backup of OUT
	if(-1==(dup2(fds[1],1))) perror("dup2()");//Out of first command goes to pipe
	execute(exec);
	if(-1==dup2(oldfdo,1)) perror("dup2()");//Restore stdout
	if(-1==close(oldfdo)) perror("close()");
	if(-1==close(fds[1])) perror("close()");
	//Right Side
	if(-1==(oldfdi=dup(0))) perror("dup");//Backup of IN
	if(-1==(dup2(fds[0],0))) perror("dup2()");//In of second command comes from pipe
	execute(cmd);
	if(-1==dup2(oldfdi,0)) perror("dup2()");//Restore stdin
	if(-1==close(oldfdi)) perror("close()");
	if(-1==close(fds[0])) perror("close()");
}
void outputRed(const char* in, const char* in2)
{
	string cmd(in);
	string file(in2);
	if(file.size()==0)
	{
		cerr << "syntax error near unexpected token'newline'" << endl;
		return;
	}
	if(file.size()!=0) removeSpaces(file);
	//Opens file for input
	int fd;
	if(-1==(fd = open(file.c_str(), O_WRONLY|O_CREAT|O_TRUNC,0666)))
	{
		perror("open()");
		exit(1);
	}
	if(isErrRed)
	{
		cmd = cmd.erase(cmd.size()-1); //Removes #2 from error redirection
		//Copies old file descriptor
		int oldfd;
		if(-1==(oldfd = dup(fileDescriptor))){
			perror("dup");
				exit(1);
		}
		//Redirect output 
		if(-1==dup2(fd,fileDescriptor)){
			perror("dup2");
			exit(1);
		}
		//Close other fd
		if(-1==close(fd)){
			perror("close()");
			exit(1);
		}
		execute(cmd);
		//Restore stdout
		if(-1==dup2(oldfd,fileDescriptor)){
			perror("dup2()");
			exit(1);
		}
		if(-1==close(oldfd)){
			perror("close()");
			exit(1);
		}
	}
	else{
		//Copies old file descriptor
		int oldfd;
		if(-1==(oldfd = dup(1))){
			perror("dup");
				exit(1);
		}
		//Redirect output
		if(-1==dup2(fd,1)){
			perror("dup2");
			exit(1);
		}
		//Close other fd
		if(-1==close(fd)){
			perror("close()");
			exit(1);
		}
		execute(cmd);
		//Restore stdout
		if(-1==dup2(oldfd,1)){
			perror("dup2()");
			exit(1);
		}
		if(-1==close(oldfd)){
			perror("close()");
			exit(1);
		}
	}

}

void outputRedAppend(const char* in, const char* in2)
{
	string cmd(in);
	string file(in2);
	if(file.size()==0)
	{
		cerr << "syntax error near unexpected token'newline'" << endl;
		return;
	}
	if(file.size()!=0) removeSpaces(file);
	//Opens file for input
	int fd;
	if(-1==(fd = open(file.c_str(), O_WRONLY|O_CREAT|O_APPEND,0666)))
	{
		perror("open()");
		exit(1);
	}
	if(isErrRed)
	{
		cmd = cmd.erase(cmd.size()-1); //Removes #2 from error redirection
		//Copies old file descriptor
		int oldfd;
		if(-1==(oldfd = dup(fileDescriptor))){
			perror("dup");
				exit(1);
		}
		//Redirect output 
		if(-1==dup2(fd,fileDescriptor)){
			perror("dup2");
			exit(1);
		}
		//Close other fd
		if(-1==close(fd)){
			perror("close()");
			exit(1);
		}
		execute(cmd);
		//Restore stdout
		if(-1==dup2(oldfd,fileDescriptor)){
			perror("dup2()");
			exit(1);
		}
		if(-1==close(oldfd)){
			perror("close()");
			exit(1);
		}
	}
	else{
		//Copies old file descriptor
		int oldfd;
		if(-1==(oldfd = dup(1))){
			perror("dup");
			exit(1);
		}
		//Redirect input 
		if(-1==dup2(fd,1)){
			perror("dup2");
			exit(1);
		}
		//Close other fd
		if(-1==close(fd)){
			perror("close()");
			exit(1);
		}
		execute(cmd);
		//Restore stdout
		if(-1==dup2(oldfd,1)){
			perror("dup2()");
			exit(1);
		}
		if(-1==close(oldfd)){
			perror("close()");
			exit(1);
		}
	}
}
