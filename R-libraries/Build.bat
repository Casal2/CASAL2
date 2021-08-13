rem Build R library and version
R --vanilla < run-roxygen.R
R --vanilla < CASAL2_make_version.R

rem Build CASAL library
rem R CMD build --force casal
rem R CMD INSTALL --build casal
rem R CMD check casal

rem Build Casal2 R Library
R CMD build --force casal2
rem R CMD INSTALL --build casal2
R CMD check Casal2

rem Build DataWeighting Library
R CMD build --force DataWeighting
rem R CMD INSTALL --build DataWeighting
R CMD check DataWeighting



