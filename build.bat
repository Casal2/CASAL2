cd BuildSystem
call doBuild.bat cleanall
call doBuild.bat version
call doBuild.bat thirdparty
call doBuild.bat library release
call doBuild.bat library betadiff
call doBuild.bat library adolc
call doBuild.bat library test
call doBuild.bat frontend
call doBuild.bat documentation
call doBuild.bat rlibrary
call doBuild.bat archive
call doBuild.bat installer
cd ..
