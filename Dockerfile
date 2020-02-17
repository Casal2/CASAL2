# see https://www.rocker-project.org/images/

# FROM rocker/r-ver:3.6.1
# FROM rocker/tidyverse:3.6.1
# FROM rocker/verse:3.6.1

# see https://github.com/rocker-org/r-parallel
# which is built on rocker/r-base, https://github.com/rocker-org/rocker/tree/master/r-base
# which uses Debian testing and the most recent R version
FROM rocker/r-parallel AS developer

ARG R_VERSION=3.6.2


# is this necessary? where is it from?
# ENV DEBIAN_FRONTEND=noninteractive


RUN apt-get update && apt-get upgrade -y \
 && apt-get -y --no-install-recommends install git git-svn openssh-client nano vim ca-certificates curl procps file time locate \
                                               bash-completion pkg-config python-pip python-dateutil cmake build-essential gawk \
                                               less graphviz subversion cpp g++ gcc gfortran clang clang-tidy \
                                               libc6-dev liblapack-dev libblas-dev libquadmath0 pandoc \
                                               unzip p7zip bibtool doxygen-latex python3-pip python3-hypothesis \
                                               fontforge-extras texlive-full texlive-font-utils texlive-latex-extra \
# for devtools dependencies
                                               libxml2-dev libudunits2-dev libgit2-dev \
                                               libssl-dev zlib1g-dev libgit2-dev libcurl4-gnutls-dev libssh2-1-dev \
# [un]comment these out later to clear caches. package caches are useful during active dev.
 && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/* \
 && pip install datetime && pip3 install datetime \
 && pip install pytz && pip3 install pytz

# RUN install.r devtools roxygen2 dplyr ggplot2 here Hmisc rlist rmarkdown huxtable mvtnorm
RUN R -e "install.packages(c('devtools', 'roxygen2', 'dplyr', 'ggplot2', 'here', 'Hmisc', 'rlist', 'rmarkdown', 'huxtable', 'mvtnorm'))" \
 && useradd --home-dir /r-script -U casal2

WORKDIR /r-script/casal2

COPY alias.txt /r-script/.alias

# RUN git clone https://git.niwa.co.nz/fisheries/modelling/casal2-development.git casal2
COPY . .

RUN cd R-libraries \
 && R -e "install.packages('casal_2.30.tar.gz', repos=NULL, type='source')" \
 && R -e "install.packages('casal2_1.0.tar.gz', repos=NULL, type='source')"

ENV DOCKER='T'

RUN chown -R casal2:casal2 /r-script/*

USER casal2

RUN cd BuildSystem && ./doBuild.sh check \
 && ./doBuild.sh thirdparty adolc && ./doBuild.sh thirdparty betadiff && ./doBuild.sh thirdparty boost \
 && ./doBuild.sh thirdparty dlib && ./doBuild.sh thirdparty googletest_googlemock && ./doBuild.sh thirdparty parser \
 && ./doBuild.sh release && ./doBuild.sh test && ./doBuild.sh release adolc && ./doBuild.sh release betadiff
# CppAD doesn't build in docker for some reason
# && ./doBuild.sh thirdparty cppad && ./doBuild.sh release cppad \
# the documentation should already exist
# && ./doBuild.sh documentation \
# the Casal2 R library should already exist
# && ./doBuild.sh rlibrary \
# && ./doBuild.sh modelrunner

RUN cd BuildSystem && ./doBuild.sh library release && ./doBuild.sh library test \
 && ./doBuild.sh library adolc && ./doBuild.sh library betadiff \
# CppAD doesn't build in docker for some reason
# && ./doBuild.sh library cppad \
 && ./doBuild.sh frontend

CMD ["/bin/bash"]

