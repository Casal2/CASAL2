cd BuildSystem
call doBuild.bat thirdparty
call doBuild.bat debug
call doBuild.bat release
call doBuild.bat test
call doBuild.bat documentation
call doBuild.bat archive
call doBuild.bat rlibrary
call doBuild.bat installer
cd ..
