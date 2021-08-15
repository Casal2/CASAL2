cd BuildSystem

doBuild.sh version

doBuild.sh thirdparty boost
doBuild.sh thirdparty adolc
doBuild.sh thirdparty betadiff
doBuild.sh thirdparty parser

doBuild.sh version

doBuild.sh release
doBuild.sh release betadiff
doBuild.sh release adolc

doBuild.sh library release
doBuild.sh library betadiff
doBuild.sh library adolc
doBuild.sh library test

doBuild.sh frontend

doBuild.sh documentation
doBuild.sh rlibrary
doBuild.sh archive
doBuild.sh installer

doBuild.sh modelrunner
doBuild.sh unittests

cd ..
