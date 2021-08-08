cd BuildSystem

rem call doBuild.bat clean_all
call doBuild.bat version

rem call doBuild.bat thirdparty boost
call doBuild.bat thirdparty adolc
call doBuild.bat thirdparty betadiff
call doBuild.bat thirdparty parser

call doBuild.bat version

call doBuild.bat release
call doBuild.bat release betadiff
call doBuild.bat release adolc

call doBuild.bat library release
call doBuild.bat library betadiff
call doBuild.bat library adolc
call doBuild.bat library test

call doBuild.bat frontend

call doBuild.bat documentation
call doBuild.bat rlibrary
call doBuild.bat archive
call doBuild.bat installer

call doBuild.bat modelrunner

cd ..
