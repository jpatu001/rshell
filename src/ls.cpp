#include<iostream>
#include<string>
#include<unistd.h>	//fork(), execvp(), etc
#include<sys/wait.h>	//wait calls
#include<stdio.h>	//perror
#include<errno.h>
#include<dirent.h>	//Directory Syscalls
#include<sys/stat.h> 
#include<sys/types.h>
#include<signal.h>	//signal(),kill(), etC
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
#include<pwd.h>
#include<grp.h>
using namespace std;

#define PRINT_HID cout << "\033[47;m";
#define PRINT_DIR cout << "\033[34m";
#define PRINT_DIRHID cout << "\033[1;47;34m";
#define PRINT_EXE cout << "\033[1;32m";
#define PRINT_EXEHID cout << "\033[1;47;32m";
#define OUT_DEFAULT cout << "\033[0;00m";

void printPerm(const string & file, const string& path);
void printFiles(const char* directory, bool& dashA, bool& dashL, bool& dashR,bool& isMultDir);
bool stringCOMP(const string& a, const string& b);

int main(int argc, char** argv)
{
	vector<string>files;
	vector<string>directory;
	vector<string>userIN;

	bool dashA = false;
	bool dashL = false;
	bool dashR = false;
	bool isMultDir = false;
	
	//NOT ENOUGH ARGV
	if(argc < 1)//Not enough Arguments passed in
	{
		cout << "No Arguments Passed" << endl;
		exit(1);
	}
	//======================================================================================
	//Multiple Arguments with optional paths
	//======================================================================================
	else
	{
		//Takes all user input, and looks for the flags 
		for(int i = 1; i < argc; i++)
		{
			if(argv[i][0]!=' ' && argv[i][0]!='-') userIN.push_back(argv[i]); //Non flag
			else if(argv[i][0]!=' ' && argv[i][0]=='-') //Possible Flags
			{
				for(int j = 1; argv[i][j]!='\0'; j++){
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
		if(userIN.size() > 0){//Sorts data into files and directories vector
			for(unsigned int i = 0; i < userIN.size(); i++)
			{
				struct stat s;
				if( ( stat(userIN.at(i).c_str(), &s ) )==-1 )
				{
					perror("stat()");
					exit(1);
				}	
				if(S_ISREG(s.st_mode)) files.push_back(userIN.at(i));//
				else if(S_ISDIR(s.st_mode))
				{
					if(userIN.at(i).at(userIN.at(i).size()-1)!='/')
					{	
						userIN.at(i) = userIN.at(i) + '/'; 
					}
					directory.push_back(userIN.at(i));
				}
			}
			//For Files
			sort(directory.begin(), directory.end(), stringCOMP);
			sort(files.begin(), files.end(), stringCOMP);
			if(files.size()>0)
			{	
				int maxWidth = 70;
				int currWidth = 0;
				for(unsigned int i = 0; i < files.size(); i++)
				{
					if(currWidth>=maxWidth)
					{
						currWidth = 0;
						cout << endl;
					}
					struct stat s;
					//=== COLOR ====================================================
					bool is_dir = false;
					bool is_exe = false;
					OUT_DEFAULT //RESETS OUTPUT COLOR
					if( ( stat(files.at(i).c_str(), &s ) )==-1 )
					{
						perror("stat()");
						exit(1);
					}
					if(S_ISDIR(s.st_mode)){
						is_dir = true;
						if(files.at(i).at(0)!='.') PRINT_DIR//DIRECTORY
						else if(files.at(i).at(0)=='.') PRINT_DIRHID//HIDDEN_DIR
					}
					if(s.st_mode & S_IXUSR && !is_dir){
						is_exe = true;
						if(files.at(i).at(0)!='.') PRINT_EXE 
						if(files.at(i).at(0)=='.') PRINT_EXEHID
					}
					if(files.at(i).at(0)=='.' && !is_exe && !is_dir) PRINT_HID
					//=== OUTPUT =================================================
					if(dashL) printPerm(files.at(i), ".");
					else{
						currWidth += files.at(i).size() + 2;
						cout << files.at(i);
						OUT_DEFAULT
						cout << "  ";

					}
				}
				cout << endl;
			}
			if(directory.size()>0)
			{	
				isMultDir = true;
				//For Directories;
				for(unsigned int i = 0; i < directory.size(); i++)
				{
					printFiles(directory.at(i).c_str(), dashA, dashL, dashR, isMultDir);
				}
			}
		}
		//==================================================================================
		//NO PATH GIVEN, DEFAULT TO ..
		//==================================================================================
		else
			{//Ouput files in the current directory
			printFiles(".", dashA, dashL, dashR, isMultDir);
		}
	}
	return 0;
}

void printPerm(const string & file, const string& path)
{
	OUT_DEFAULT
	struct stat s;
	bool isExe = false;
	bool isDir = false;
	if( ( stat(path.c_str(), &s ) )==-1 )
	{
		perror("stat()");
		exit(1);
	}	
	//=== IDs ================================	
	struct group  *group;
	struct passwd *passwd;
	bool idFailed = false;
	group = getgrgid(s.st_gid); 
	if(group==NULL){
		idFailed = true;
		perror("getgrid()");
	}
	passwd = getpwuid(s.st_uid);
	if(passwd==NULL){
		idFailed = true;
		perror("getpwuid()");
	}
	//========================================

	//Access Permissions
	if(S_ISREG(s.st_mode)) cout << "-";
	else if(S_ISDIR(s.st_mode)){
		isDir = true;
		cout << "d";
	}
	else if(S_ISCHR(s.st_mode)) cout << "c";
	else if(S_ISBLK(s.st_mode)) cout << "b";
	else if(S_ISLNK(s.st_mode)) cout << "l";

	cout << ((s.st_mode & S_IRUSR) ? "r" : "-");					
	cout << ((s.st_mode & S_IWUSR) ? "w" : "-");
	cout << ((s.st_mode & S_IXUSR) ? "x" : "-");
	cout << ((s.st_mode & S_IRGRP) ? "r" : "-");					
	cout << ((s.st_mode & S_IWGRP) ? "w" : "-");
	cout << ((s.st_mode & S_IXGRP) ? "x" : "-");
	cout << ((s.st_mode & S_IROTH) ? "r" : "-");					
	cout << ((s.st_mode & S_IWOTH) ? "w" : "-");
	cout << ((s.st_mode & S_IXOTH) ? "x" : "-");
	//===Number of Hard Links==============
	cout << setw(2) << s.st_nlink << " ";
	//===TIME===============================

	time_t t = s.st_mtime;
	struct tm lt;
	localtime_r(&t, &lt);
	char time[80];
	strftime(time,sizeof(time), "%c", &lt);
	if(!idFailed){
		cout << setw(8) << passwd->pw_name;
		cout << setw(8) << group->gr_name;
	}
	else if(idFailed){
		cout << setw(8) << "USER";
		cout << setw(8) << "GROUP";
	}
	cout << setw(8) << s.st_size << " ";
	cout << setw(20) << time << " ";
	//===COLORS=============================
	if(isDir)
	{
		if(file.at(0)!='.') PRINT_DIR
		if(file.at(0)=='.') PRINT_DIRHID
	}
	if(s.st_mode & S_IXUSR && !isDir)
	{
		isExe = true;
		if(file.at(0)!='.') PRINT_EXE
		if(file.at(0)=='.') PRINT_EXEHID

	}
	if(file.at(0)=='.' && !isDir && !isExe) PRINT_HID
	//===OUTPUT=============================
	cout << file;
	OUT_DEFAULT
	cout << endl;
}

void printFiles(const char* directory, bool& dashA, bool& dashL, bool& dashR,bool& isMultDir)
{
	vector<string>files;
	vector<string>dir;
	int totalBlocks = 0;
	
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
		if(!dashA){//No Hidden files, ignore . files
		if(filespecs->d_name[0]!='.') files.push_back(filespecs->d_name);
			else continue;
		}
		else files.push_back(filespecs->d_name);
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

//=== -R setup ============================================================
	for(unsigned int i = 0; i < files.size(); i++)
	{
		struct stat s;
		string directory2(directory);
		string tmp(directory);
		if(directory2.at(directory2.size()-1)!='/') tmp+='/';
		tmp+= files.at(i);
		if( ( stat(tmp.c_str(), &s ) )==-1 )
		{
			perror("stat()");
			exit(1);
		}	
		//BLOCK SIZE
		if(!dashA && files.at(i).at(0)!='.') totalBlocks+=(s.st_blocks);
		else if(dashA) totalBlocks+=(s.st_blocks);	
		
	 	if(S_ISDIR(s.st_mode) && files.at(i)!=".." && files.at(i)!=".")
		{
			if(files.at(i).at(files.at(i).size()-1)!='/')
			{	
				files.at(i) = files.at(i) + '/'; 
			}

			char currDir[1024];
			if((getcwd(currDir, sizeof(currDir))==NULL)) perror("getcwd");
			dir.push_back(tmp);
		}
	}
//========================================================================
	sort(files.begin(), files.end(), stringCOMP);
	sort(dir.begin(), dir.end(), stringCOMP);
	int maxW = 70;
	int currW = 0;
	unsigned int w = 0;
	//GETS MAXIMUM SIZE OF STRINGS
	for(unsigned int i = 0; i < files.size(); i++)
	{
		if((files.at(i).size()) > w) w = files.at(i).size();
	}
	if(dashR || isMultDir) cout << directory << ":" << endl;
	if(dashL) cout << "total " << (totalBlocks/2) << endl;
	for(unsigned int i = 0; i < files.size(); i++)
	{
 		struct stat s;
 		string directory2(directory);
		string tmp(directory);
		if(directory2.at(directory2.size()-1)!='/') tmp+='/';
		tmp+= files.at(i);
		if( ( stat(tmp.c_str(), &s ) )==-1 )
		{
			perror("stat()");
			exit(1);
		}	
		//=== COLOR ====================================================
		bool is_dir = false;
		bool is_exe = false;
		OUT_DEFAULT //RESETS OUTPUT COLOR
		if(S_ISDIR(s.st_mode)){
			is_dir = true;
			if(files.at(i).at(0)!='.') PRINT_DIR//DIRECTORY
			else if(files.at(i).at(0)=='.') PRINT_DIRHID//HIDDEN_DIR
		}
		if(s.st_mode & S_IXUSR && !is_dir){
			is_exe = true;
			if(files.at(i).at(0)!='.') PRINT_EXE 
			if(files.at(i).at(0)=='.') PRINT_EXEHID
		}
		if(files.at(i).at(0)=='.' && !is_exe && !is_dir) PRINT_HID
		//=============================================================
		if(!dashL){
			if(currW>=maxW){
				cout << endl;
				currW = 0;
			}
			currW+= files.at(i).size()+1;
			cout << setw(w) << left << files.at(i);
			OUT_DEFAULT //Resets cout color
			cout << "  ";
		} 
		else if(dashL){
			string directory2(directory);
			string tmp(directory);
			if(directory2.at(directory2.size()-1)!='/') tmp+='/';
			tmp+= files.at(i);
			printPerm(files.at(i), tmp);
		}
		OUT_DEFAULT //Resets cout color
		if(files.size()==i -1) cout << endl << endl;
	}
	if(!dashL) cout << endl;
	//RECURSIVE PRINTFILES: for -R
	if(dashR)
	{	
		for(unsigned int i = 0; i < dir.size(); i++)
		{
			printFiles(dir.at(i).c_str(), dashA, dashL, dashR, isMultDir);
		}
	}
}

bool stringCOMP(const string& a, const string& b)
{
	unsigned int i = 0, j = 0;
	//IGNORE . files
	if((a.size()>1) && (a.at(0)=='.')) i++;
	if((b.size()>1) && (b.at(0)=='.')) j++;

	while( (i < a.size()) && (j < b.size()) )
	{
		if( (tolower(a.at(i))) < (tolower(b.at(j))) ) return true;
		else if( (tolower(a.at(i))) > (tolower(b.at(j))) ) return false;

		i++;
		j++;
	}
	return (a.size() < b.size());
}
