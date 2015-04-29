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
#include<errno.h>
#include<algorithm>
#include<iomanip>
using namespace std;


void printFiles(const char* directory, bool dashA, bool dashL, bool dashR)
{
	vector<string>files;
	vector<string>directories;

	if(dashA && !dashL && !dashR)// -A
	{
		DIR *dirp;
		if(NULL == (dirp = opendir(directory)))
		{
			perror("There was an error with opendir(). ");
			exit(1);
		}
				
		struct dirent *filespecs;
		errno = 0;
		while(NULL != (filespecs = readdir(dirp)))
		{
			files.push_back(filespecs->d_name);
		}
		if(errno != 0)
		{
			perror("There was an error with readdir(). ");
			exit(1);
		}
		if(-1 == closedir(dirp))
		{
			perror("There was an error with closedir(). ");
			exit(1);
		}
		sort(files.begin(), files.end());
		for(unsigned int i = 0; i < files.size(); i++)
		{
			cout << files.at(i) << " ";
		}
		cout << endl;
	}
	else if(!dashA && !dashL && !dashR)//No Flag
	{
		DIR *dirp;
		if(NULL == (dirp = opendir(directory)))
		{
			perror("There was an error with opendir(). ");
			exit(1);
		}
				
		struct dirent *filespecs;
		errno = 0;
		while(NULL != (filespecs = readdir(dirp)))
		{
			if(filespecs->d_name[0]!='.') files.push_back(filespecs->d_name);
			else continue;
		}
		if(errno != 0)
		{
			perror("There was an error with readdir(). ");
			exit(1);
		}
		if(-1 == closedir(dirp))
		{
			perror("There was an error with closedir(). ");
			exit(1);
		}
		sort(files.begin(), files.end());
		for(unsigned int i = 0; i < files.size(); i++)
		{
			cout << files.at(i) << " ";
		}
		cout << endl;

	}

	else
	{
		cout << "Flag Not Supported Yet" << endl;
	}

}

int main(int argc, char** argv)
{
	//vector<string>files;
	//vector<string>directory;
	vector<string>userIN;

	bool dashA = false;
	bool dashL = false;
	bool dashR = false;

	if(argc < 1)
	{
		cout << "No Arguments Passed" << endl;
		exit(1);
	}
	else
	{
		//Takes all user input, but drops all flags 
		for(int i = 1; i < argc; i++)
		{
			if(argv[i][0]!=' ' && argv[i][0]!='-') userIN.push_back(argv[i]); //Non flag
			else if(argv[i][0]!=' ' && argv[i][0]=='-') //Possible Flags
			{
				for(int j = 1; argv[i][j]!='\0'; j++){
					//cout << "ARG: " << argv[i][j] << endl;
					if(argv[i][j]=='a') dashA = true;
					else if(argv[i][j]=='l') dashL = true;
					else if(argv[i][j]=='R') dashR = true;
					else{
						cerr << "Invalid Flag" << endl;
						exit(1);
					}
				}
			}
			else{}
		}
		if(userIN.size() > 0){	
			printFiles(userIN.at(0).c_str(), dashA, dashL, dashR);
		}
		else{//Ouput files in the current directory
			printFiles(".", dashA, dashL, dashR);
		}
		
	}

	return 0;
}
