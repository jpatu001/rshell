# rshell
Basic command shell called RSHELL. Outputs the current user on the terminal including the host name and '$' sign. It accepts commands and executes them. This is a work in progress so several bugs waiting to be fixed exists. Anything after comment will be removed from the command argument.


#Installation Guide
```
$ git clone  https://github.com/jpatu001/rshell.git


$ cd rshell


$ git checkout hw0


$ make


$ bin/rshell
```


#Bugs/ Limitations
##Invalid Connectors terminates rshell
*(5 or more consecutive instances of && or ||)
```
ls -a &&&&& touch test
ls |||||||| ls -a -l
```
*(Multiple instances of ';' without actual commands in between)
```
ls;;;;;;; pwd
ls -l -a;;
```
*(Connectors with no arguments)
```
;;;;
&&&&&&&
```
##Accepts multiple consecutive connectors
```
pwd ||| ls
ls &&&& pwd
```

##Cannot handle commands insde () or ""
```
"ls"
(pwd)
```
