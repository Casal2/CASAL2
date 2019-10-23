# see https://www.rocker-project.org/images/

FROM rocker/r-ver:3.6.1
# FROM rocker/tidyverse:3.6.1
# FROM rocker/verse:3.6.1

ARG R_VERSION=3.6.1

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y \
 && apt-get -y --no-install-recommends install git git-svn openssh-client nano vim ca-certificates curl procps file time locate \
                                               bash-completion pkg-config python-pip python-dateutil cmake build-essential \
                                               texlive-full cpp g++ gcc gfortran clang clang-tidy \
                                               libc6-dev liblapack-dev libblas-dev libquadmath0 \
                                               unzip p7zip bibtool doxygen-latex \
                                               fontforge-extras texlive-font-utils texlive-latex-extra \
# for devtools dependencies
                                               libxml2-dev libudunits2-dev libgit2-dev \
                                               libssl-dev zlib1g-dev libgit2-dev libcurl4-gnutls-dev libssh2-1-dev \
# [un]comment these out later to clear caches. package caches are useful during active dev.
 && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/* \
 && pip install datetime \
 && R -e "install.packages(c('devtools', 'roxygen2', 'dplyr', 'ggplot2', 'here', 'Hmisc'))" \
 && useradd --home-dir /r-script -U casal2

WORKDIR /r-script/casal2

COPY alias.txt /r-script/.alias

# RUN git clone https://git.niwa.co.nz/fisheries/modelling/casal2-development.git casal2
COPY . .

ENV DOCKER='T'

RUN chown -R casal2:casal2 /r-script/*

USER casal2

RUN cd BuildSystem && ./doBuild.sh check && ./doBuild.sh documentation \
  && ./doBuild.sh thirdparty adolc && ./doBuild.sh thirdparty betadiff && ./doBuild.sh thirdparty boost \
  && ./doBuild.sh thirdparty dlib && ./doBuild.sh thirdparty googletest_googlemock && ./doBuild.sh thirdparty parser \
  && ./doBuild.sh release betadiff && ./doBuild.sh test
#  && ./doBuild.sh thirdparty cppad && ./doBuild.sh release cppad
#  && ./doBuild.sh release adolc && ./doBuild.sh release betadiff && ./doBuild.sh release cppad && ./doBuild.sh test
#  && ./doBuild.sh rlibrary && ./doBuild.sh modelrunner && ./doBuild.sh install

CMD ["/bin/bash"]
