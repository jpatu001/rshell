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
using namespace std;

int main()
{
 	string name;
	name = getlogin();
	if(name=="")
	{
		perror("getlogin()");
	}

	//Retrieves hostname
	char host[64];
	gethostname(host, 64);	
	if (gethostname(host, sizeof(host-1))==-1)
	{
		perror("gethostname()");
	}	
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
	
	string str;
	while(1)
	{
		cout << name << "$ ";
        cin >> str;
        if(str=="exit")
		{
			exit(1);
		}
		else
		{
			continue;
		}
	}




	return 0;
}
