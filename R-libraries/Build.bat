R --vanilla < run-roxygen.R
R --vanilla < CASAL2_make_version.R

Rcmd build --force CASAL2
Rcmd INSTALL --build CASAL2

del CASAL2.html




