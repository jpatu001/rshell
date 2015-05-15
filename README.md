# rshell
Basic command shell called RSHELL. Outputs the current user on the terminal including the host name and '$' sign. It accepts commands and executes them. It also accepts multiple commands with connectors such as `||`, `&&` and `;`. Any command after `;` will be executed regardless of previous commands. Commands after `&&` will be executed only if the previous command succeeded. Commands after `||`will execute only if the previous command failed. This is a work in progress so several bugs waiting to be fixed exists. Anything after comment will be removed from the command argument. Typing `exit` exits the shell but `exit` should be the first command(anything after exit is ignore). `cd` is currently unsupported. (Under Tag: hw0)

Added ls. (Under Tag: hw1). It works like the bash ls. It only support -l -a -R or any combination of the three flags but anything other than that, it will produce an error. It also accepts and optional file parameters.

Added Pipe and IO Redirection(Under Tag: hw2). Supports `<` `>` `>>` `2>` or `2>>` for redirection. 

#Installation Guide
```
$ git clone  https://github.com/jpatu001/rshell.git


$ cd rshell


$ git checkout hw0
//for ls, use `git checkout hw1`

$ make


$ bin/rshell
//for ls, use `bin/ls`
```


#Bugs/ Limitations
###Invalid Connectors terminates rshell
* (5 or more consecutive instances of && or ||)
```
ls -a &&&&& touch test
ls |||||||| ls -a -l
```
* (Multiple instances of ';' without actual commands in between)
```
ls;;;;;;; pwd
ls -l -a;;
```
* (Connectors with no arguments)
```
;;;;
&&&&&&&
```


###Accepts multiple consecutive connectors
```
pwd ||| ls
ls &&&& pwd
```


###Cannot handle commands insde () or ""
```
"ls"
(pwd)
```

###`true`  evaluates as true, and `false` evaluates as true as well
```
false && ls //This executes LS either way.
```


###Throws an error message(perror) when a command failed, even when using connectors.
```
awd || ls
```
The code above excecutes `ls` since the first one failed(invalid) command, but it throws the perror.


###Empty commands before connectors always evaluates as true
```
&& ls
|| ls
; ls
```
###IO Redirection
* The program exits when the input file is invalid, but throws a perror. Ex: `cat < nonexistingfile`
* Reads '<<' as '<' 

##Bugs/ Limitations for ls
* Sorting is not the same as from bash
* Output is not left to right instead of top to bottom(bash)
* Occasion of do_ypcall, probably a server problem
* getpwuid sometimes recieves NULL, in this case, GROUP/ USER is used as default
* Cannot handle flags or optional arguments with spaces
```
bin/ls -a        -l    -R
```
