# Лабораторна робота myshell

## Структура LL(1) граматики:
```java
<line>           		::=   	<cmd><com>|<com>
<cmd>               		::=   	<dot><prg><args>|<var>'='<valnullable>
<com>          		::=   	'#'*|<eps>
<prg>          		::=   	<dir>'/'<prg>|<prgname>
<prgname>          		::=   	<literal>
<args>          		::=   	<arg><args>|<eps>
<arg>          		::=   	<val>|<key>'='<val>|<flag>
<var>          		::=   	<literal>
<val>          		::=   	<varcall>|<prg>
<valnulable>          		::=   	<val>|<eps>
<varcall>			::=	'$'<var>
<dir>	         		::=	(['0'-'9']+['a'-'z']+['A'-'Z']+'_'+'~'+'.'+'..')(['0'-'9']+['a'-'z']+['A'-'Z']+'_'+'-')*
<key>          		::=	(['0'-'9']+['a'-'z']+['A'-'Z']+'_')(['0'-'9']+['a'-'z']+['A'-'Z']+'_')*
<flag>          		::=	('-'+'--')(['0'-'9']+['a'-'z']+['A'-'Z']+'_')(['0'-'9']+['a'-'z']+['A'-'Z']+'_')*
<literal>          		::=	(['0'-'9']+['a'-'z']+['A'-'Z']+'_'+'.')*
```


## Приклад команд myshell
```php
DATA =  dir1/dir2

#test call of myprg
../myprg   arg1=$DATA --help        # TODO: remove --help
```

