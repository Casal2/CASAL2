CASAL2 README

Firstly the developers would like to thank you for downloading CASAL2 and remind you that this is open source, and contributions in ideas and to source code is incouraged. This download once unzipped should contain, A manual (CASAL2.pdf), an executable (casal2.exe), Source code (src), an examples folder and 5 dynamic libraries (casal2_adolc.dll, casal2_betadiff.dll ...etc).

Running Casal2 for windows users
If you can change your systems path, it is advised to put the directory that casal2.exe and all the the thirdparty libraries with the extension .dll into your systems path. Once you have  casal2 in your path if you open the command prompt in any directory (shift + rigth click -> open command window) and type casal2 -h, you can get the help screen from casal2. If CASAL2 is in your path then open the command prompt in the Examples/Simple folder. To do a simple run type "casal2 -r" which will print output to the screen. If you want to just run CASAL2 then open the command prompt in the folder that casal2.exe and libraries and type the command into the command prompt "casal2 -r -c Examples/Simple/casal2.txt", which will also print output to the screen. for other run modes use the following commands:
Estimation
casal2 -e
Profile a specific parameter
casal2 -p
MCMC run
casal2 -m
simulate 1 set of observations
casal2 -s 1
one set of projections
casal2 -f 1

To 

For an overview of CASAL2 we suggest you start by reading, getting started with CASAL2.pdf. This will explain how to run CASAL2, and the fundamental syntax for CASAL2. For a more indepth read users are recommended to go to the manual.
