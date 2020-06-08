# msvcr14x

## prior project:
https://github.com/sonyps5201314/ntdll.git

## build steps：
1. git clone https://github.com/sonyps5201314/msvcr14x.git  
2. open msvcr14x.sln with visual studio 2017.  
3. batch build all configurations of all projects except msvcrx project.  
In first time build, you can not build msvcp14x succuess, remove CONCRT14X(D).lib in 'Linker'->'Input'->'Additional Dependencies' options of msvcp14x project, after build it, build concrt14x, then revert the 'Additional Dependencies' options of msvcp14x project. build all except msvcrx. now you will succuess.  
Not first time build, CONCRT14X(D).lib is in each output directory, so you can build all succuess.  
msvcrx project is experimental project,so do not use it,it can be removed in the future.  
  
## use steps:
1. After build out all dlls(MSVCR14X(D).dll,MSVCP14X(D).dll,CONCRT14X(D).dll). set a system environment variable with name is "msvcr14x_ROOT" and value is 'your repo directory'.  
2. Explorer navigate to 'C:\Users\%USERNAME%\AppData\Local\Microsoft\MSBuild\v4.0\' directory, replace "Microsoft.Cpp.Win32.user.props" and "Microsoft.Cpp.x64.user.props" with the same files in your directory. now all finished. rebuild your vc solution you will find your app no longer dependent on vc crt dlls and app-sets dlls, enjoy it.  
3.copy all generated dlls(MSVCR14X(D).dll,MSVCP14X(D).dll,CONCRT14X(D).dll) to 'C:\Windows\SysWOW64'(for x86 32bit programs), and to 'C:\Windows\System32'(for x64 64bit programs), is a good idea for test and debug.

## refer to:
https://source.winehq.org/git/wine.git
https://github.com/Microsoft/STL
