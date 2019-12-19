# see https://www.rocker-project.org/images/

FROM rocker/r-ver:3.6.1
# FROM rocker/tidyverse:3.6.1
# FROM rocker/verse:3.6.1

ARG R_VERSION=3.6.1

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y \
 && apt-get -y --no-install-recommends install git git-svn openssh-client nano vim ca-certificates curl procps file time locate \
                                               bash-completion pkg-config python-pip python-dateutil cmake build-essential gawk \
                                               less graphviz subversion cpp g++ gcc gfortran clang clang-tidy \
                                               libc6-dev liblapack-dev libblas-dev libquadmath0 \
                                               unzip p7zip bibtool doxygen-latex python3-pip python3-hypothesis \
                                               fontforge-extras texlive-full texlive-font-utils texlive-latex-extra \
# for devtools dependencies
                                               libxml2-dev libudunits2-dev libgit2-dev \
                                               libssl-dev zlib1g-dev libgit2-dev libcurl4-gnutls-dev libssh2-1-dev \
# [un]comment these out later to clear caches. package caches are useful during active dev.
 && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/* \
 && pip install datetime && pip3 install datetime \
 && pip install pytz && pip3 install pytz \
 && R -e "install.packages(c('devtools', 'roxygen2', 'dplyr', 'ggplot2', 'here', 'Hmisc'))" \
 && useradd --home-dir /r-script -U casal2

WORKDIR /r-script/casal2

COPY alias.txt /r-script/.alias

# RUN git clone https://git.niwa.co.nz/fisheries/modelling/casal2-development.git casal2
COPY . .

ENV DOCKER='T'

RUN chown -R casal2:casal2 /r-script/*

USER casal2

RUN cd BuildSystem && ./doBuild.sh check \
 && ./doBuild.sh thirdparty adolc && ./doBuild.sh thirdparty betadiff && ./doBuild.sh thirdparty boost \
 && ./doBuild.sh thirdparty dlib && ./doBuild.sh thirdparty googletest_googlemock && ./doBuild.sh thirdparty parser \
 && ./doBuild.sh release && ./doBuild.sh test && ./doBuild.sh release adolc && ./doBuild.sh release betadiff \
# && ./doBuild.sh thirdparty cppad && ./doBuild.sh release cppad \
# && ./doBuild.sh documentation \
 && ./doBuild.sh rlibrary \
 && ./doBuild.sh modelrunner

RUN cd BuildSystem && ./doBuild.sh library release && ./doBuild.sh library test \
 && ./doBuild.sh library adolc && ./doBuild.sh library betadiff
# && ./doBuild.sh library cppad \
 && ./doBuild.sh frontend && ./doBuild.sh deb

CMD ["/bin/bash"]

