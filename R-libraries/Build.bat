R --vanilla < run-roxygen.R
R --vanilla < iSAM_make_version.R

Rcmd build --force iSAM
Rcmd INSTALL --build iSAM

#copy iSAM_*.zip \\niwa.local\groups\Wellington\NIWAFisheries\R\ /Y
#copy iSAM_*.tar.gz \\niwa.local\groups\Wellington\NIWAFisheries\R\ /Y
#copy iSAM.html \\niwa.local\groups\Wellington\NIWAFisheries\R\ /Y
del iSAM.html




