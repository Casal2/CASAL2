FROM rocker/r-ver:3.6.1

ARG R_VERSION=3.6.1

LABEL maintainer="Ryan Darby <ryan.darby@niwa.co.nz>"

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get upgrade -y \
 && apt-get -y --no-install-recommends install git git-svn openssh-client nano vim ca-certificates curl procps time \
                                               python-pip gfortran texlive-full p7zip cmake build-essential cpp g++ gcc gfortran \
                                               clang unzip bibtool python-dateutil clang-tidy doxygen-latex \
                                               fontforge-extras texlive-font-utils texlive-latex-extra bash-completion pkg-config

RUN pip install datetime

# for devtools dependencies
RUN apt-get -y --no-install-recommends install libxml2-dev \
                                               # libcurl4-openssl-dev libssl-dev zlib1g-dev libgit2-dev \
                                               libssl-dev zlib1g-dev libgit2-dev libcurl4-gnutls-dev libssh2-1-dev

# TODO uncomment these later to clear caches. package caches are useful during active dev.
#  && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/*
RUN R -e "install.packages('devtools')"
RUN R -e "install.packages('roxygen2')"
RUN R -e "install.packages('Rcpp')"
RUN R -e "install.packages('ggplot2')"
RUN R -e "library(devtools)"
RUN R -e "library(roxygen2)"
RUN R -e "library(Rcpp)"
RUN R -e "library(ggplot2)"
WORKDIR /r-script

COPY docker.alias.txt /r-script/.alias

RUN uname -a

RUN git clone https://github.com/NIWAFisheriesModelling/CASAL2.git casal2

RUN cd /r-script/casal2/BuildSystem && ./doBuild.sh check

