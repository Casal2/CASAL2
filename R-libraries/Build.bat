R --vanilla < run-roxygen.R
R --vanilla < CASAL2_make_version.R

R CMD build --force casal2
R CMD INSTALL --build casal2
R CMD check CASAL2

del CASAL2.html




