# rshell
Basic command shell called RSHELL. Outputs the current user on the terminal including the host name and '$' sign. It accepts commands and executes them. It also accepts multiple commands with connectors such as `||`, `&&` and `;`. Any command after `;` will be executed regardless of previous commands. Commands after `&&` will be executed only if the previous command succeeded. Commands after `||`will execute only if the previous command failed. This is a work in progress so several bugs waiting to be fixed exists. Anything after comment will be removed from the command argument. Typing `exit` exits the shell but `exit` should be the first command(anything after exit is ignore). `cd` is currently unsupported.


#Installation Guide
```
$ git clone  https://github.com/jpatu001/rshell.git


$ cd rshell


$ git checkout hw0


$ make


$ bin/rshell
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
