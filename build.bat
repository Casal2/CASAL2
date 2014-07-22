cd BuildSystem
call doBuild.bat thirdparty
call doBuild.bat debug
call doBuild.bat release
call doBuild.bat unittests
call doBuild.bat documentation
cd ..
